<template>
  <v-container fluid>
    <v-row class="text-center">
      <v-col
       :cols="heaterColumnWidth"
       class="d-flex"
       style="flex-direction:column"
      >
        <v-card outlined tile elevation="10" class="ma-2 flex-grow-1">
          <v-card-text>
            <svg viewBox="0 0 800 400">
              <HeaterIcon
               x="0" y="0"
               :iconColor="flameDetected ? 'red' : 'black'"
               width="800"
               height="400"/>

              <FanIcon
               x="650" y="140"
               width="120"
               height="120"
               onColor="blue"
               offColor="black"
               :running="this.fanRunning"
               @click="fanDialogOpen = true"/>

              <GlowPlugIcon
               x="400" y="65"
               width="120"
               height="120"
               offColor="black"
               onColor="red"
               :running="this.glowPlugOn"
               @click="glowPlugDialogOpen = true"
              />

              <FireIcon
               x="180" y="80"
               width="250"
               height="250"
               :running="this.flameDetected"
               @click="heaterDialogOpen = true"/>

              <PumpIcon
               x="500" y="65"
               width="120"
               height="120"
               onColor="blue"
               offColor="black"
               :running="this.pumpRunning"
               @click="pumpDialogOpen = true"/>

              <svg x="110" y="70" width="215" height="40">
                <line x1="0" y1="0" x2="215" y2="0" style="stroke:black; stroke-width: 76"/>  
                <text x="5" y="25" style="font: bold 25px sans-serif;fill: red;">{{this.stateStr}}</text>
              </svg>

              <svg x="30" y="180" width="130" height="40">
                <line x1="0" y1="0" x2="130" y2="0" style="stroke:black; stroke-width: 76"/>  
                <text x="5" y="30" style="font: bold 30px sans-serif;fill: red;">{{this.outletTemp.toFixed(1)}} {{tempUnit}}</text>
              </svg>

              <svg x="620" y="265" width="90" height="40">
                <line x1="0" y1="0" x2="90" y2="0" style="stroke:black; stroke-width: 76"/>  
                <text x="5" y="30" style="font: bold 30px sans-serif;fill: red;">{{this.fanPower}}%</text>
              </svg>

              <svg x="600" y="80" width="100" height="40">
                <line x1="0" y1="0" x2="100" y2="0" style="stroke:black; stroke-width: 76"/>  
                <text x="5" y="30" style="font: bold 30px sans-serif;fill: red;">{{this.pumpFreq.toFixed(1)}} Hz</text>
              </svg>
            </svg>
          </v-card-text>
        </v-card>
      </v-col>

      <v-col
       v-if="rightPanelColumnWidth === 4"
       :cols="rightPanelColumnWidth"
       class="d-flex"
       style="flex-direction:column"
      >
        <v-row class="ma-0">
          <TempChart class="ma-2 flex-grow-1"
           title="Outlet Temperature"
           :unit="tempUnit"
           :value="outletTemp"
           :trend="outletTempTrends"
          />
        </v-row>

        <v-row class="ma-0">
          <TempChart class="ma-2 flex-grow-1"
           title="Room Temperature"
           :unit="tempUnit"
           :value="roomTemp"
           :trend="roomTempTrends"
          />
        </v-row>
      </v-col>

      <v-col
       v-if="rightPanelColumnWidth === 12"
       class="ma-0"
      >
        <v-row dense class="ma-1">
          <v-col cols="6" class="ma-0">
            <TempChart class="ma-0" style="height: 400"
             title="Outlet Temperature"
             :unit="tempUnit"
             :value="outletTemp"
             :trend="outletTempTrends"
            />
          </v-col>

          <v-col cols="6" class="ma-0" style="height: 400">
            <TempChart class="ma-0"
             title="Room Temperature"
             :unit="tempUnit"
             :value="roomTemp"
             :trend="roomTempTrends"
            />
          </v-col>
        </v-row>
      </v-col>
    </v-row>

    <GlowPlugDialog
     v-model="glowPlugDialogOpen"
     @on="onGlowPlugOnClick"
     @off="onGlowPlugOffClick"
    />

    <PumpDialog
     v-model="pumpDialogOpen"
     @start="onPumpStartClick"
     @stop="onPumpStopClick"
     @freq="onPumpFreqChange"
    />

    <FanDialog
     v-model="fanDialogOpen"
     @start="onFanStartClick"
     @stop="onFanStopClick"
     @power="onFanPowerChange"
    />

    <HeaterDialog
     v-model="heaterDialogOpen"
     @start="onHeaterStartClick"
     @stop="onHeaterStopClick"
    />

    <LoaderDialog
     key="heater-loader"
     :open="loaderOpen"
     :message="loaderMessage"
    />

    <SettingsDialog
     v-model="settingsDialogOpen"
    />

    <CommPortDialog
     v-model="commPortDialogOpen"
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
  </v-container>
</template>

<script>
  import FanIcon from './FanIcon'
  import GlowPlugIcon from './GlowPlugIcon'
  import FireIcon from './FireIcon'
  import PumpIcon from './PumpIcon'
  import HeaterIcon from './HeaterIcon'
  import GlowPlugDialog from './GlowPlugDialog'
  import PumpDialog from './PumpDialog'
  import FanDialog from './FanDialog'
  import HeaterDialog from './HeaterDialog'
  import LoaderDialog from './LoaderDialog'
  import SettingsDialog from './SettingsDialog'
  import CommPortDialog from './CommPortDialog'
  import TempChart from './TempChart'
  import { mapGetters } from 'vuex'

  export default {
    name: 'Heater',
    components: {
      FanIcon,
      GlowPlugIcon,
      FireIcon,
      PumpIcon,
      HeaterIcon,
      GlowPlugDialog,
      PumpDialog,
      FanDialog,
      HeaterDialog,
      LoaderDialog,
      SettingsDialog,
      CommPortDialog,
      TempChart,
    },
    data: () => ({
      glowPlugDialogOpen: false,
      pumpDialogOpen: false,
      fanDialogOpen: false,
      heaterDialogOpen: false,
      loaderOpen: false,
      loaderMessage: 'Please standby',
      snackbar: false,
      snackbarMsg: '',
      settingsDialogOpen: false,
      commPortDialogOpen: false,
    }),
    methods: {
      //
      // utilities
      //
      showSnackBar(msg) {
        this.snackbarMsg = msg 
        this.snackbar = true
      },
      showLoader(msg) {
        this.loaderMessage = msg
        this.loaderOpen = true
      },
      closeLoader() {
        this.loaderOpen = false
      },
      //
      // glow plug
      //
      onGlowPlugOnClick() {
        let self = this

        this.showLoader('Turning on glow plug...')

        this.$store.dispatch('glowOn', {
          callback: (err) => {
            if (err) {
              self.showSnackBar('Failed to turn on glow plug')
            } 
            self.closeLoader()
          }
        })
      },
      onGlowPlugOffClick() {
        let self = this

        this.showLoader('Turning off glow plug...')

        this.$store.dispatch('glowOff', {
          callback: (err) => {
            if (err) {
              self.showSnackBar('Failed to turn off glow plug')
            } 
            self.closeLoader()
          }
        });
      },
      //
      // pump
      //
      onPumpStartClick() {
        let self = this

        this.showLoader('Starting Pump...')

        this.$store.dispatch('pumpStart', {
          callback: (err) => {
            if (err) {
              self.showSnackBar('Failed to start pump')
            } 
            self.closeLoader()
          }
        });
      },
      onPumpStopClick() {
        let self = this

        this.showLoader('Stopping Pump...')

        this.$store.dispatch('pumpStop', {
          callback: (err) => {
            if (err) {
              self.showSnackBar('Failed to stop pump')
            } 
            self.closeLoader()
          }
        });
      },
      onPumpFreqChange(freq) {
        let self = this

        this.showLoader('Changing Pump Frequency to ' + freq.toFixed(1) + 'Hz...')

        this.$store.dispatch('pumpFreq', {
          callback: (err) => {
            if (err) {
              self.showSnackBar('Failed to set fan power')
            } 
            self.closeLoader()
          },
          freq
        });
      },
      //
      // fan
      //
      onFanStartClick() {
        let self = this

        this.showLoader('Starting Fan...')

        this.$store.dispatch('fanStart', {
          callback: (err) => {
            if (err) {
              self.showSnackBar('Failed to start fan')
            } 
            self.closeLoader()
          }
        });
      },
      onFanStopClick() {
        let self = this

        this.showLoader('Stopping Fan...')

        this.$store.dispatch('fanStop', {
          callback: (err) => {
            if (err) {
              self.showSnackBar('Failed to stop fan')
            } 
            self.closeLoader()
          }
        });
      },
      onFanPowerChange(power) {
        let self = this

        this.showLoader('Changing Fan Power to ' + power + '%...')

        this.$store.dispatch('fanPower', {
          callback: (err) => {
            if (err) {
              self.showSnackBar('Failed to set fan power')
            } 
            self.closeLoader()
          },
          power
        });
      },
      //
      // Heater
      //
      onHeaterStartClick() {
        let self = this

        this.showLoader('Starting Heater...')

        this.$store.dispatch('heaterStart', {
          callback: (err) => {
            if (err) {
              self.showSnackBar('Failed to start heater')
            } 
            self.closeLoader()
          }
        });
      },
      onHeaterStopClick() {
        let self = this

        this.showLoader('Stopping Heater...')

        this.$store.dispatch('heaterStop', {
          callback: (err) => {
            if (err) {
              self.showSnackBar('Failed to stop heater')
            } 
            self.closeLoader()
          }
        });
      },
      //
      // Settings
      //
      openSettingsDialog() {
        this.settingsDialogOpen = true
      },
      openCommPortDialog() {
/// #if RUN_TARGET == 'electron'
        this.commPortDialogOpen = true
/// #endif
      },
    },
    mounted: function () {
/// #if RUN_TARGET == 'web'
      this.$store.dispatch('startPolling')
/// #endif
    },
    computed: {
      ...mapGetters([
        'fanRunning',
        'glowPlugOn',
        'flameDetected',
        'pumpRunning',
        'outletTemp',
        'fanPower',
        'pumpFreq',
        'stateStr',
        'outletTempTrends',
        'roomTemp',
        'roomTempTrends',
        'tempUnit',
      ]),
      heaterColumnWidth() {
        switch (this.$vuetify.breakpoint.name) {
          case 'xs':
          case 'sm':
            return 12

          case 'md':
          case 'ld':
          case 'xl':
            break;
        }
        return 8
      },
      rightPanelColumnWidth() {
        switch (this.$vuetify.breakpoint.name) {
          case 'xs':
          case 'sm':
            return 12

          case 'md':
          case 'ld':
          case 'xl':
            break;
        }
        return 4
      },
    },
  }
</script>
