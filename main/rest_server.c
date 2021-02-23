/* HTTP Restful API Server

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>
#include <fcntl.h>
#include <math.h>
#include "esp_http_server.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_vfs.h"
#include "cJSON.h"

#include "app_heater.h"

static const char *REST_TAG = "esp-rest";
#define REST_CHECK(a, str, goto_tag, ...)                                              \
    do                                                                                 \
    {                                                                                  \
        if (!(a))                                                                      \
        {                                                                              \
            ESP_LOGE(REST_TAG, "%s(%d): " str, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
            goto goto_tag;                                                             \
        }                                                                              \
    } while (0)

#define FILE_PATH_MAX (ESP_VFS_PATH_MAX + 128)
#define SCRATCH_BUFSIZE (10240)

typedef struct rest_server_context {
  char base_path[ESP_VFS_PATH_MAX + 1];
  char scratch[SCRATCH_BUFSIZE];
} rest_server_context_t;

#define CHECK_FILE_EXTENSION(filename, ext) (strcasecmp(&filename[strlen(filename) - strlen(ext)], ext) == 0)

/* Set HTTP response content type according to file extension */
static esp_err_t set_content_type_from_file(httpd_req_t *req, const char *filepath)
{
  const char *type = "text/plain";
  if (CHECK_FILE_EXTENSION(filepath, ".html")) {
    type = "text/html";
  } else if (CHECK_FILE_EXTENSION(filepath, ".js")) {
    type = "application/javascript";
  } else if (CHECK_FILE_EXTENSION(filepath, ".css")) {
    type = "text/css";
  } else if (CHECK_FILE_EXTENSION(filepath, ".png")) {
    type = "image/png";
  } else if (CHECK_FILE_EXTENSION(filepath, ".ico")) {
    type = "image/x-icon";
  } else if (CHECK_FILE_EXTENSION(filepath, ".svg")) {
    type = "text/xml";
  }
  return httpd_resp_set_type(req, type);
}

#if 0
static
esp_err_t rest_common_get_handler(httpd_req_t *req)
{
  char filepath[FILE_PATH_MAX];

  rest_server_context_t *rest_context = (rest_server_context_t *)req->user_ctx;
  strlcpy(filepath, rest_context->base_path, sizeof(filepath));

  if (req->uri[strlen(req->uri) - 1] == '/') {
    strlcat(filepath, "/index.html", sizeof(filepath));
  } else {
    strlcat(filepath, req->uri, sizeof(filepath));
  }
  int fd = open(filepath, O_RDONLY, 0);
  if (fd == -1) {
    ESP_LOGE(REST_TAG, "Failed to open file : %s", filepath);
    /* Respond with 500 Internal Server Error */
    httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to read existing file");
    return ESP_FAIL;
  }

  set_content_type_from_file(req, filepath);

  char *chunk = rest_context->scratch;
  ssize_t read_bytes;
  do {
    /* Read file in chunks into the scratch buffer */
    read_bytes = read(fd, chunk, SCRATCH_BUFSIZE);
    if (read_bytes == -1) {
      ESP_LOGE(REST_TAG, "Failed to read file : %s", filepath);
    } else if (read_bytes > 0) {
      /* Send the buffer contents as HTTP response chunk */
      if (httpd_resp_send_chunk(req, chunk, read_bytes) != ESP_OK) {
        close(fd);
        ESP_LOGE(REST_TAG, "File sending failed!");
        /* Abort sending file */
        httpd_resp_sendstr_chunk(req, NULL);
        /* Respond with 500 Internal Server Error */
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to send file");
        return ESP_FAIL;
      }
    }
  } while (read_bytes > 0);
  /* Close file after sending complete */
  close(fd);
  ESP_LOGI(REST_TAG, "File sending complete");
  /* Respond with an empty chunk to signal HTTP response completion */
  httpd_resp_send_chunk(req, NULL, 0);
  return ESP_OK;
}
#endif

static
esp_err_t rest_common_br_compressed_get_handler(httpd_req_t *req)
{
  char filepath[FILE_PATH_MAX];

  rest_server_context_t *rest_context = (rest_server_context_t *)req->user_ctx;

  strlcpy(filepath, rest_context->base_path, sizeof(filepath));

  if (req->uri[strlen(req->uri) - 1] == '/') {
    strlcat(filepath, "/index.html", sizeof(filepath));
  } else {
    strlcat(filepath, req->uri, sizeof(filepath));
  }
  set_content_type_from_file(req, filepath);

  int fd = open(filepath, O_RDONLY, 0);
  if (fd == -1) {
    //
    // append ".gz" for compressed files
    //
    strlcat(filepath, ".gz", sizeof(filepath));
    fd = open(filepath, O_RDONLY, 0);
    if (fd == -1) {
      ESP_LOGE(REST_TAG, "Failed to open file : %s", filepath);
      /* Respond with 500 Internal Server Error */
      httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to read existing file");
      return ESP_FAIL;
    }
    httpd_resp_set_hdr(req, "Content-Encoding", "gzip");
  }

  char *chunk = rest_context->scratch;
  ssize_t read_bytes;
  do {
    /* Read file in chunks into the scratch buffer */
    read_bytes = read(fd, chunk, SCRATCH_BUFSIZE);
    if (read_bytes == -1) {
      ESP_LOGE(REST_TAG, "Failed to read file : %s", filepath);
    } else if (read_bytes > 0) {
      /* Send the buffer contents as HTTP response chunk */
      if (httpd_resp_send_chunk(req, chunk, read_bytes) != ESP_OK) {
        close(fd);
        ESP_LOGE(REST_TAG, "File sending failed : %s", filepath);
        /* Abort sending file */
        httpd_resp_sendstr_chunk(req, NULL);
        /* Respond with 500 Internal Server Error */
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to send file");
        return ESP_FAIL;
      }
    }
  } while (read_bytes > 0);
  /* Close file after sending complete */
  close(fd);
  ESP_LOGI(REST_TAG, "File sending complete: %s", filepath);
  /* Respond with an empty chunk to signal HTTP response completion */
  httpd_resp_send_chunk(req, NULL, 0);
  return ESP_OK;
}

static
esp_err_t system_info_get_handler(httpd_req_t *req)
{
  httpd_resp_set_type(req, "application/json");
  cJSON *root = cJSON_CreateObject();
  esp_chip_info_t chip_info;
  esp_chip_info(&chip_info);
  cJSON_AddStringToObject(root, "version", IDF_VER);
  cJSON_AddNumberToObject(root, "cores", chip_info.cores);
  const char *sys_info = cJSON_Print(root);
  httpd_resp_sendstr(req, sys_info);
  free((void *)sys_info);
  cJSON_Delete(root);
  return ESP_OK;
}

////////////////////////////////////////////////////////////////////////////////
//
// for Heater REST interface
//
////////////////////////////////////////////////////////////////////////////////
/* Simple handler for getting heater status */
static const char*
heater_get_common_json_status(void)
{
	static app_heater_status_rsp_t    r;

	app_heater_status(&r);

  cJSON *root = cJSON_CreateObject();

  cJSON_AddNumberToObject(root, "state", r.state);
  cJSON_AddNumberToObject(root, "fanPower", r.fan_pwr);
  cJSON_AddBoolToObject(root, "fanRunning", r.fan_on);
  cJSON_AddBoolToObject(root, "flameDetected", r.flame_detected);
  cJSON_AddBoolToObject(root, "glowPlugOn", r.glow_on);
  cJSON_AddNumberToObject(root, "outletTemp", r.outlet_temp);
  cJSON_AddNumberToObject(root, "roomTemp", r.room_temp);
  cJSON_AddNumberToObject(root, "pumpFreq", r.oil_freq);
  cJSON_AddBoolToObject(root, "pumpRunning", r.oil_on);

  const char *status = cJSON_Print(root);

  cJSON_Delete(root);

  return status;
}

static const char*
heater_get_common_json_settings(void)
{
	static settings_t s;

	app_heater_get_settings(&s);

  cJSON *root = cJSON_CreateObject();

  cJSON_AddNumberToObject(root, "s0", s.glow_plug_on_duration_for_start);
  cJSON_AddNumberToObject(root, "s1", s.oil_pump_priming_duration);
  cJSON_AddNumberToObject(root, "s2", s.glow_plug_on_duration_for_stop);
  cJSON_AddNumberToObject(root, "s3", s.cooling_down_period);
  cJSON_AddNumberToObject(root, "s4", s.startup_fan_power);
  cJSON_AddNumberToObject(root, "s5", s.stop_fan_power);
  cJSON_AddNumberToObject(root, "s6", s.glow_plug_pwm_freq);
  cJSON_AddNumberToObject(root, "s7", s.glow_plug_pwm_duty);
  cJSON_AddNumberToObject(root, "s8", s.oil_pump_startup_freq);
  cJSON_AddNumberToObject(root, "s9", s.oil_pump_pulse_length);
  cJSON_AddArrayToObject(root, "steps");

  cJSON* array = cJSON_GetObjectItem(root, "steps");

	for(uint8_t i = 0; i < MAX_OIL_PUMP_FAN_STEPS; i++)
	{
    cJSON* item;

    item = cJSON_CreateObject();
    cJSON_AddNumberToObject(item, "pump_freq", s.steps[i].pump_freq);
    cJSON_AddNumberToObject(item, "fan_pwr", s.steps[i].fan_pwr);

    cJSON_AddItemToArray(array, item);
	}

  const char *status = cJSON_Print(root);

  cJSON_Delete(root);

  return status;
}

static esp_err_t
retrieve_post_content(httpd_req_t* req, char** b)
{
  int total_len = req->content_len;
  int cur_len = 0;
  char *buf = ((rest_server_context_t *)(req->user_ctx))->scratch;
  int received = 0;

  if (total_len >= SCRATCH_BUFSIZE) {
    /* Respond with 500 Internal Server Error */
    httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "content too long");
    return ESP_FAIL;
  }

  while (cur_len < total_len) {
    received = httpd_req_recv(req, buf + cur_len, total_len);
    if (received <= 0) {
      /* Respond with 500 Internal Server Error */
      httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to post control value");
      return ESP_FAIL;
    }
    cur_len += received;
  }
  buf[total_len] = '\0';
  *b = buf;

  return ESP_OK;
}

//
// heater status get
//
static
esp_err_t heater_status_get_handler(httpd_req_t *req)
{
  httpd_resp_set_type(req, "application/json");

  const char *status = heater_get_common_json_status(); 
  httpd_resp_sendstr(req, status);
  free((void *)status);

  return ESP_OK;
}

static
esp_err_t heater_settings_get_handler(httpd_req_t* req)
{
  httpd_resp_set_type(req, "application/json");

  const char *status = heater_get_common_json_settings();
  httpd_resp_sendstr(req, status);

  free((void *)status);

  return ESP_OK;
}

static esp_err_t
heater_fan_start_post_handler(httpd_req_t *req)
{
  app_heater_control_fan(true);

  httpd_resp_set_type(req, "application/json");

  const char *status = heater_get_common_json_status(); 
  httpd_resp_sendstr(req, status);
  free((void *)status);

  return ESP_OK;
}

static esp_err_t
heater_fan_stop_post_handler(httpd_req_t *req)
{
  app_heater_control_fan(false);

  httpd_resp_set_type(req, "application/json");

  const char *status = heater_get_common_json_status(); 
  httpd_resp_sendstr(req, status);
  free((void *)status);

  return ESP_OK;
}

static esp_err_t
heater_fan_power_post_handler(httpd_req_t *req)
{
  char* buf;

  if(retrieve_post_content(req, &buf) != ESP_OK)
  {
    return ESP_FAIL;
  }

  cJSON *root = cJSON_Parse(buf);

  int power = cJSON_GetObjectItem(root, "power")->valueint;
  cJSON_Delete(root);

  app_heater_control_fan_power(power);

  httpd_resp_set_type(req, "application/json");

  const char *status = heater_get_common_json_status(); 
  httpd_resp_sendstr(req, status);
  free((void *)status);

  return ESP_OK;
}

static esp_err_t
heater_pump_start_post_handler(httpd_req_t *req)
{
  app_heater_control_oil_pump(true);

  httpd_resp_set_type(req, "application/json");

  const char *status = heater_get_common_json_status(); 
  httpd_resp_sendstr(req, status);
  free((void *)status);

  return ESP_OK;
}

static esp_err_t
heater_pump_stop_post_handler(httpd_req_t *req)
{
  app_heater_control_oil_pump(false);

  httpd_resp_set_type(req, "application/json");

  const char *status = heater_get_common_json_status(); 
  httpd_resp_sendstr(req, status);
  free((void *)status);

  return ESP_OK;
}

static esp_err_t
heater_pump_freq_post_handler(httpd_req_t *req)
{
  char* buf;

  if(retrieve_post_content(req, &buf) != ESP_OK)
  {
    return ESP_FAIL;
  }

  cJSON *root = cJSON_Parse(buf);

  float freq = (float)cJSON_GetObjectItem(root, "freq")->valuedouble;
  cJSON_Delete(root);

  app_heater_control_oil_pump_freq(freq);

  httpd_resp_set_type(req, "application/json");

  const char *status = heater_get_common_json_status(); 
  httpd_resp_sendstr(req, status);
  free((void *)status);

  return ESP_OK;
}

static esp_err_t
heater_glow_plug_on_post_handler(httpd_req_t *req)
{
  app_heater_control_glow(true);

  httpd_resp_set_type(req, "application/json");

  const char *status = heater_get_common_json_status(); 
  httpd_resp_sendstr(req, status);
  free((void *)status);

  return ESP_OK;
}

static esp_err_t
heater_glow_plug_off_post_handler(httpd_req_t *req)
{
  app_heater_control_glow(false);

  httpd_resp_set_type(req, "application/json");

  const char *status = heater_get_common_json_status(); 
  httpd_resp_sendstr(req, status);
  free((void *)status);

  return ESP_OK;
}

static esp_err_t
heater_heater_start_post_handler(httpd_req_t *req)
{
  app_heater_start();

  httpd_resp_set_type(req, "application/json");

  const char *status = heater_get_common_json_status(); 
  httpd_resp_sendstr(req, status);
  free((void *)status);

  return ESP_OK;
}

static esp_err_t
heater_heater_stop_post_handler(httpd_req_t *req)
{
  app_heater_stop();

  httpd_resp_set_type(req, "application/json");

  const char *status = heater_get_common_json_status(); 
  httpd_resp_sendstr(req, status);
  free((void *)status);

  return ESP_OK;
}

static esp_err_t
heater_heater_settings_mod_post_handler(httpd_req_t* req)
{
  char* buf;

  if(retrieve_post_content(req, &buf) != ESP_OK)
  {
    return ESP_FAIL;
  }

  cJSON *root = cJSON_Parse(buf);
  static app_heater_settings_req_t    r;

  r.item = (int)cJSON_GetObjectItem(root, "ndx")->valueint;

  switch(r.item)
  {
  case 0: // glow plug on duration for start
    r.u32 = (int)cJSON_GetObjectItem(root, "value")->valueint;
    break;

  case 1: // oil pump priming duration
    r.u32 = (int)cJSON_GetObjectItem(root, "value")->valueint;
    break;

  case 2: // glow plug on duration for stop
    r.u32 = (int)cJSON_GetObjectItem(root, "value")->valueint;
    break;

  case 3: // cooling down period
    r.u32 = (int)cJSON_GetObjectItem(root, "value")->valueint;
    break;

  case 4: // start up fan power
    r.u8 = (int)cJSON_GetObjectItem(root, "value")->valueint;
    break;

  case 5: // stop fan power
    r.u8 = (int)cJSON_GetObjectItem(root, "value")->valueint;
    break;

  case 6: // glow plug PWM frequency
    r.u8 = (int)cJSON_GetObjectItem(root, "value")->valueint;
    break;

  case 7: // glow plug PWM duty
    r.u8 = (int)cJSON_GetObjectItem(root, "value")->valueint;
    break;

  case 8: // oil pump frequency
    r.f = (float)cJSON_GetObjectItem(root, "value")->valuedouble;
    break;

  case 9: // oil pump pulse length
    r.u8 = (int)cJSON_GetObjectItem(root, "value")->valueint;
    break;

  default:
    break;
  }

  cJSON_Delete(root);

  app_heater_mod_settings(&r);
  app_heater_save();

  httpd_resp_set_type(req, "application/json");

  const char *status = heater_get_common_json_settings();
  httpd_resp_sendstr(req, status);

  free((void *)status);

  return ESP_OK;
}

static esp_err_t
heater_heater_settings_step_post_handler(httpd_req_t* req)
{
  char* buf;

  if(retrieve_post_content(req, &buf) != ESP_OK)
  {
    return ESP_FAIL;
  }

  cJSON *root = cJSON_Parse(buf);
  static app_heater_step_req_t    r;

  r.ndx = (int)cJSON_GetObjectItem(root, "ndx")->valueint;
  r.pwr = (int)cJSON_GetObjectItem(root, "pwr")->valueint;
  r.freq = (float)cJSON_GetObjectItem(root, "freq")->valuedouble;

  cJSON_Delete(root);

  app_heater_step(&r);
  app_heater_save();

  httpd_resp_set_type(req, "application/json");

  const char *status = heater_get_common_json_settings();
  httpd_resp_sendstr(req, status);

  free((void *)status);

  return ESP_OK;
}

////////////////////////////////////////////////////////////////////////////////
//
// REST server main
//
////////////////////////////////////////////////////////////////////////////////
esp_err_t start_rest_server(const char *base_path)
{
  REST_CHECK(base_path, "wrong base path", err);
  rest_server_context_t *rest_context = calloc(1, sizeof(rest_server_context_t));
  REST_CHECK(rest_context, "No memory for rest context", err);
  strlcpy(rest_context->base_path, base_path, sizeof(rest_context->base_path));

  httpd_handle_t server = NULL;
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  config.max_uri_handlers = 24;
  config.lru_purge_enable  = true;
  config.uri_match_fn = httpd_uri_match_wildcard;

  ESP_LOGI(REST_TAG, "Starting HTTP Server");
  REST_CHECK(httpd_start(&server, &config) == ESP_OK, "Start server failed", err_start);

  /* URI handler for fetching system info */
  httpd_uri_t system_info_get_uri = {
    .uri = "/api/v1/system/info",
    .method = HTTP_GET,
    .handler = system_info_get_handler,
    .user_ctx = rest_context
  };
  httpd_register_uri_handler(server, &system_info_get_uri);

  /////////////////////////////////////////////////////////////////////
  //
  // Heater REST APIs
  //
  /////////////////////////////////////////////////////////////////////
  httpd_uri_t heater_status_get_uri = {
    .uri = "/api/v1/heater/status",
    .method = HTTP_GET,
    .handler = heater_status_get_handler,
    .user_ctx = rest_context
  };
  httpd_register_uri_handler(server, &heater_status_get_uri);

  httpd_uri_t heater_settings_get_uri = {
    .uri = "/api/v1/heater/settings",
    .method = HTTP_GET,
    .handler = heater_settings_get_handler,
    .user_ctx = rest_context
  };
  httpd_register_uri_handler(server, &heater_settings_get_uri);

  httpd_uri_t heater_fan_start_post_uri = {
    .uri = "/api/v1/heater/fan/start",
    .method = HTTP_POST,
    .handler = heater_fan_start_post_handler,
    .user_ctx = rest_context
  };
  httpd_register_uri_handler(server, &heater_fan_start_post_uri);

  httpd_uri_t heater_fan_stop_post_uri = {
    .uri = "/api/v1/heater/fan/stop",
    .method = HTTP_POST,
    .handler = heater_fan_stop_post_handler,
    .user_ctx = rest_context
  };
  httpd_register_uri_handler(server, &heater_fan_stop_post_uri);

  httpd_uri_t heater_fan_power_post_uri = {
    .uri = "/api/v1/heater/fan/power",
    .method = HTTP_POST,
    .handler = heater_fan_power_post_handler,
    .user_ctx = rest_context
  };
  httpd_register_uri_handler(server, &heater_fan_power_post_uri);

  httpd_uri_t heater_pump_start_post_uri = {
    .uri = "/api/v1/heater/pump/start",
    .method = HTTP_POST,
    .handler = heater_pump_start_post_handler,
    .user_ctx = rest_context
  };
  httpd_register_uri_handler(server, &heater_pump_start_post_uri);

  httpd_uri_t heater_pump_stop_post_uri = {
    .uri = "/api/v1/heater/pump/stop",
    .method = HTTP_POST,
    .handler = heater_pump_stop_post_handler,
    .user_ctx = rest_context
  };
  httpd_register_uri_handler(server, &heater_pump_stop_post_uri);

  httpd_uri_t heater_pump_freq_post_uri = {
    .uri = "/api/v1/heater/pump/freq",
    .method = HTTP_POST,
    .handler = heater_pump_freq_post_handler,
    .user_ctx = rest_context
  };
  httpd_register_uri_handler(server, &heater_pump_freq_post_uri);

  httpd_uri_t heater_glow_plug_on_post_uri = {
    .uri = "/api/v1/heater/glowplug/on",
    .method = HTTP_POST,
    .handler = heater_glow_plug_on_post_handler,
    .user_ctx = rest_context
  };
  httpd_register_uri_handler(server, &heater_glow_plug_on_post_uri);

  httpd_uri_t heater_glow_plug_off_post_uri = {
    .uri = "/api/v1/heater/glowplug/off",
    .method = HTTP_POST,
    .handler = heater_glow_plug_off_post_handler,
    .user_ctx = rest_context
  };
  httpd_register_uri_handler(server, &heater_glow_plug_off_post_uri);

  httpd_uri_t heater_heater_start_post_uri = {
    .uri = "/api/v1/heater/start",
    .method = HTTP_POST,
    .handler = heater_heater_start_post_handler,
    .user_ctx = rest_context
  };
  httpd_register_uri_handler(server, &heater_heater_start_post_uri);

  httpd_uri_t heater_heater_stop_post_uri = {
    .uri = "/api/v1/heater/stop",
    .method = HTTP_POST,
    .handler = heater_heater_stop_post_handler,
    .user_ctx = rest_context
  };
  httpd_register_uri_handler(server, &heater_heater_stop_post_uri);

  httpd_uri_t heater_heater_settings_mod_post_uri = {
    .uri = "/api/v1/heater/settings/mod",
    .method = HTTP_POST,
    .handler = heater_heater_settings_mod_post_handler,
    .user_ctx = rest_context
  };
  httpd_register_uri_handler(server, &heater_heater_settings_mod_post_uri);

  httpd_uri_t heater_heater_settings_step_post_uri = {
    .uri = "/api/v1/heater/settings/step",
    .method = HTTP_POST,
    .handler = heater_heater_settings_step_post_handler,
    .user_ctx = rest_context
  };
  httpd_register_uri_handler(server, &heater_heater_settings_step_post_uri);

  /* URI handler for getting web server files */
  httpd_uri_t common_get_uri = {
    .uri = "/*",
    .method = HTTP_GET,
    //.handler = rest_common_get_handler,
    .handler = rest_common_br_compressed_get_handler,
    .user_ctx = rest_context
  };
  httpd_register_uri_handler(server, &common_get_uri);


  return ESP_OK;
err_start:
  free(rest_context);
err:
  return ESP_FAIL;
}
