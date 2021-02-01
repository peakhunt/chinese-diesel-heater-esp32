<template>
  <v-dialog
   :value="value"
   @input="$emit('input', $event)"
   eager
   fullscreen
   transition="dialog-bottom-transition"
  >
    <v-card>
      <v-toolbar color="primary">
        <v-btn outlined @click="$emit('input', false)">X</v-btn>
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
       v-model="editSettingOpen"
       :desc="editSettingDesc"
       :setting="editSetting"
       @change="onEditSettingChange"
      />

      <EditStepDialog
       v-model="editStepOpen"
       :desc="editStep"
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
      onEditSettingChange(obj) {
        const { desc, setValue } = obj
        const self = this

        self.editSettingOpen = false
        self.showLoader(`updating ${desc.name}...`)
        self.$store.dispatch('heaterSettingChange',
          {
            ndx: desc.ndx,
            value: desc.isInt ? setValue * desc.divider : setValue,
            callback: (settings, err) => {
              self.closeLoader()

              if (err) {
                self.showSnackbar(`failed to update ${desc.name}`)
              } else {
                self.settings = settings
              }
            }
          }
        )
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

        self.$store.dispatch('heaterStepChange',
          {
            ndx: desc.index,
            pwr: power,
            freq: freq,
            callback: (settings, err) => {
              self.closeLoader()

              if (err) {
                self.showSnackbar(`failed to update step-${desc.index}`)
              } else {
                self.settings = settings
              }
            }
          }
        )
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
    }),
    props: {
      value: {
        type: Boolean,
        default: false,
      },
    },
    watch: {
      value: function(newVal) {
        let self = this

        if (newVal) {
          self.settings = null
          self.errorOnLoadSettings = false
          self.showLoader('retrieving settings data')

          self.$store.dispatch('heaterSettingsGet', {
            callback: (settings, err) => {
              self.closeLoader()

              if (err) {
                self.errorOnLoadSettings = true
                self.showSnackbar('failed to retrieve settings from server')
              } else {
                self.settings = settings 
              }
            }
          })
        }
      },
    },
  }
</script>
