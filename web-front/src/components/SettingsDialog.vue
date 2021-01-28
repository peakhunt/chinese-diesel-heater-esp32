<template>
  <v-dialog
   :value="open"
   eager
   fullscreen
   transition="dialog-bottom-transition"
   @click:outside="$emit('close')"
   @keydown.esc="$emit('close')"
  >
    <v-card>
      <v-toolbar color="primary">
        <v-btn outlined @click="$emit('close')">X</v-btn>
        <v-spacer></v-spacer>
        <v-toolbar-title>Settings</v-toolbar-title>
      </v-toolbar>

      <v-card-text>
        <v-subheader>
          Client Settings
        </v-subheader>
        <v-container fluid>
          <v-row align="center">
            <v-col cols="2">
              <v-subheader>Temperature Unit"</v-subheader>
            </v-col>
            <v-col cols="2">
              <v-select
               :value="tempUnit"
               :items="tempUnits"
               single-line
               label="Temperature Unit"
               @change="onTempUnitChange"
              ></v-select>
            </v-col>
          </v-row>
        </v-container>

        <div v-if="settings !== null">
          <v-subheader>
            Heater Settings
          </v-subheader>
          <v-simple-table dense>
            <template v-slot:default>
              <thead>
                <tr>
                  <th class="text-left">
                    Name
                  </th>
                  <th class="text-left">
                    Value
                  </th>
                </tr>
              </thead>
              <tbody>
                <tr
                 v-for="item in settingItems"
                 :key="item.name"
                 @click="onSettingClick(item)"
                >
                  <td>{{item.name}}</td>
                  <td>{{ item.isInt ? settings['s' + item.ndx] / item.divider : settings['s' + item.ndx].toFixed(1)}} {{ item.unit }}</td>
                </tr>
              </tbody>
            </template>
          </v-simple-table>

          <v-subheader>
            Steps
          </v-subheader>
          <v-simple-table dense>
            <template v-slot:default>
              <thead>
                <tr>
                  <th class="text-left">
                    Name
                  </th>
                  <th class="text-left">
                    Fan Power
                  </th>
                  <th class="text-left">
                    Fuel Pump Frequency
                  </th>
                </tr>
              </thead>
              <tbody>
                <tr
                 v-for="(item,index) in [0, 1, 2, 3, 4]"
                 :key="index"
                 @click="onStepClick(index, settings.steps[index])"
                >
                  <td>step-{{index}}</td>
                  <td>{{settings.steps[index].fan_pwr}} %</td>
                  <td>{{settings.steps[index].pump_freq.toFixed(1)}} Hz</td>
                </tr>
              </tbody>
            </template>
          </v-simple-table>
        </div>
        <div v-if="errorOnLoadSettings">
          <h1>Failed to load settings</h1>
        </div>

      </v-card-text>

      <LoaderDialog
       :message="loaderMsg"
       :open="loaderOpen"
      ></LoaderDialog>

      <EditSettingDialog
       :open="editSettingOpen"
       :desc="editSettingDesc"
       :setting="editSetting"
       @close="onEditSettingClose"
       @change="onEditSettingChange"
      />

      <EditStepDialog
       :open="editStepOpen"
       :desc="editStep"
       @close="onEditStepClose"
       @change="onEditStepChange"
      />

      <v-snackbar v-model="snackbar">
        {{ snackbarMsg }}

        <template v-slot:action="{ attrs }">
          <v-btn
           color="pink"
           text
           v-bind="attrs"
           @click="snackbar = false"
          >
            Close
          </v-btn>
        </template>
      </v-snackbar>
    </v-card>
  </v-dialog>
</template>

<script>
  import LoaderDialog from './LoaderDialog'
  import EditSettingDialog from './EditSettingDialog'
  import EditStepDialog from './EditStepDialog'
  import { mapGetters } from 'vuex'

  export default {
    name: 'SettingsDialog',
    components: {
      LoaderDialog,
      EditSettingDialog,
      EditStepDialog,
    },
    computed: {
      ...mapGetters([
        'tempUnit',
      ]),
    },
    methods: {
      onTempUnitChange() {
        this.$store.commit('TOGGLE_TEMP_UNIT')
      },
      showSnackbar(msg) {
        this.snackbar = true
        this.snackbarMsg = msg
      },
      showLoader(msg) {
        this.loaderMsg = msg
        this.loaderOpen = true
      },
      closeLoader() {
        this.loaderOpen = false
      },
      onSettingClick(item) {
        const sName = 's' + item.ndx

        this.editSettingDesc = item
        this.editSetting = item.isInt ? this.settings[sName] / item.divider : this.settings[sName]
        this.editSettingOpen = true
      },
      onEditSettingClose() {
        this.editSettingOpen = false
      },
      onEditSettingChange(obj) {
        const { desc, setValue } = obj
        const self = this

        self.editSettingOpen = false
        self.showLoader(`updating ${desc.name}...`)
        self.axios.post('/api/v1/heater/settings/mod',
          {
            ndx: desc.ndx,
            value: desc.isInt ? setValue * desc.divider : setValue,
          })
        .then((response) => {
            if (response.status !== 200) {
              self.showSnackbar(`failed to update ${desc.name}`)
              self.closeLoader()
              return
            }
            self.settings = response.data
            self.closeLoader()
        })
        .catch(function() {
          self.showSnackbar(`failed to update ${desc.name}`)
          self.closeLoader()
        })
      },
      onEditStepClose() {
        this.editStepOpen = false
      },
      onStepClick(index, step) {
        this.editStep = { index, step }
        this.editStepOpen = true
      },
      onEditStepChange(obj) {
        const { desc, power, freq} = obj
        const self = this

        self.editStepOpen = false
        self.showLoader(`updating step-${desc.index}...`)
        self.axios.post('/api/v1/heater/settings/step',
          {
            ndx: desc.index,
            pwr: power,
            freq: freq,
          })
        .then((response) => {
            if (response.status !== 200) {
              self.showSnackbar(`failed to update step-${desc.index}`)
              self.closeLoader()
              return
            }
            self.settings = response.data
            self.closeLoader()
        })
        .catch(function() {
          self.showSnackbar(`failed to update step-${desc.index}`)
          self.closeLoader()
        })
      },
    },
    data: () => ({
      settings: null,
      errorOnLoadSettings: false,
      loaderMsg: '',
      loaderOpen: false,
      snackbar: false,
      snackbarMsg: '',
      editSettingOpen: false,
      editSettingDesc: null,
      editSetting: 0,
      editStepOpen: false,
      editStep: null,
      tempUnits: [
        '°C',
        '°F',
      ],
      settingItems: [
        {
          ndx: 0,
          isInt: true,
          divider: 1000,
          name: 'glow plug on duration for start',
          unit: 'sec',
          min: 30,
          max: 150,
          step: 1,
        },
        {
          ndx: 1,
          isInt: true,
          divider: 1000,
          name: 'oil pump priming duration',
          unit: 'sec',
          min: 5,
          max: 60,
          step: 1,
        },
        {
          ndx: 2,
          isInt: true,
          divider: 1000,
          name: 'glow plug on duration for stop',
          unit: 'sec',
          min: 20,
          max: 60,
          step: 1,
        },
        {
          ndx: 3,
          isInt: true,
          divider: 1000,
          name: 'cooling down period',
          unit: 'sec',
          min: 60,
          max: 180,
          step: 1,
        },
        {
          ndx: 4,
          isInt: true,
          divider: 1,
          name: 'start-up fan power',
          unit: '%',
          min: 10,
          max: 100,
          step: 1,
        },
        {
          ndx: 5,
          isInt: true,
          divider: 1,
          name: 'stop fan power',
          unit: '%',
          min: 10,
          max: 100,
          step: 1,
        },
        {
          ndx: 6,
          isInt: true,
          divider: 1,
          name: 'glow plug PWM frequency',
          unit: 'Hz',
          min: 2,
          max: 20,
          step: 1,
        },
        {
          ndx: 7,
          isInt: true,
          divider: 1,
          name: 'glow plug PWM duty',
          unit: '%',
          min: 5,
          max: 95,
          step: 1,
        },
        {
          ndx: 8,
          isInt: false,
          name: 'oil pump startup frequency',
          unit: 'Hz',
          min: 0.8,
          max: 5.0,
          step: 0.1,
        },
        {
          ndx: 9,
          isInt: true,
          divider: 1,
          name: 'oil pump pulse length',
          unit: 'msec',
          min: 10,
          max: 100,
          step: 1,
        },
      ],
      demoSettings: {
        s0: 30000,
        s1: 7000,
        s2: 50000,
        s3: 60000,
        s4: 30,
        s5: 70,
        s6: 6,
        s7: 70,
        s8: 2.5,
        s9: 50,
        steps: [
          {
            pump_freq: 1.0,
            fan_pwr: 45,
          },
          {
            pump_freq: 1.5,
            fan_pwr: 55,
          },
          {
            pump_freq: 2.0,
            fan_pwr: 65,
          },
          {
            pump_freq: 2.5,
            fan_pwr: 70,
          },
          {
            pump_freq: 3.0,
            fan_pwr: 75,
          },
        ],
      }
    }),
    props: {
      open: {
        type: Boolean,
        default: false,
      },
    },
    watch: {
      open: function(newVal) {
        let self = this

        if (newVal) {
          self.settings = null
          self.errorOnLoadSettings = false
          self.showLoader('retrieving settings data')

          self.axios.get('/api/v1/heater/settings').then((response) => {
            if (response.status !== 200) {
              self.closeLoader()
              self.errorOnLoadSettings = true
              self.showSnackbar('failed to retrieve settings from server')
              return;
            }
            self.settings = response.data
            self.closeLoader()
          })
          .catch(function () {
            self.closeLoader()
            self.showSnackbar('failed to retrieve settings from server')
            self.errorOnLoadSettings = true
            //
            // XXX
            // for test. 
            //
            //self.settings = self.demoSettings
          })
        }
      },
    },
  }
</script>
