<template>
  <v-container>
    <v-row class="text-center">
      <v-col
       cols="8"
       class="d-flex"
       style="flex-direction:column"
      >
        <v-card outlined tile elevation="10" class="ma-2 flex-grow-1">
          <v-card-text>
            <svg viewBox="0 0 800 400">
              <HeaterIcon
               x="0" y="0"
               :iconColor="heaterColor"
               width="800"
               height="400"/>

              <FanIcon
               x="650" y="140"
               width="120"
               height="120"
               onColor="blue"
               offColor="black"
               :running="this.fanRunning"/>

              <GlowPlugIcon
               x="400" y="65"
               width="120"
               height="120"
               offColor="black"
               onColor="red"
               :running="this.glowPlugOn"/>

              <FireIcon
               x="180" y="80"
               :visibility="flameVisibility"
               width="250"
               height="250"
               :running="this.flameDetected"/>

              <PumpIcon
               x="500" y="65"
               width="120"
               height="120"
               onColor="blue"
               offColor="black"
               :running="this.pumpRunning"/>

              <svg x="110" y="70" width="130" height="40">
                <line x1="0" y1="0" x2="130" y2="0" style="stroke:black; stroke-width: 76"/>  
                <text x="5" y="30" style="font: bold 30px sans-serif;fill: red;">{{this.stateStr}}</text>
              </svg>

              <svg x="30" y="180" width="130" height="40">
                <line x1="0" y1="0" x2="130" y2="0" style="stroke:black; stroke-width: 76"/>  
                <text x="5" y="30" style="font: bold 30px sans-serif;fill: red;">{{this.outletTemp.toFixed(1)}} °C</text>
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
       cols="4"
       class="d-flex"
       style="flex-direction:column"
      >
        <v-card outlined tile elevation="10" class="ma-2 flex-grow-1">
          <v-container>
            <v-row dense>
              <v-col cols="6">
                <v-card color="#385F73" dark>
                  <v-card-title>Heater</v-card-title>
                  <v-card-actions>
                    <v-btn text>Start</v-btn>
                    <v-btn text>Stop</v-btn>
                  </v-card-actions>
                </v-card>
              </v-col>

              <v-col cols="6">
                <v-card color="pink darken-2" dark>
                  <v-card-title>Glow Plug</v-card-title>
                  <v-card-actions>
                    <v-btn text>Start</v-btn>
                    <v-btn text>Stop</v-btn>
                  </v-card-actions>
                </v-card>
              </v-col>

              <v-col cols="12">
                <v-card color="#1F7087" dark>
                  <v-card-title>Fan</v-card-title>
                  <v-slider v-model="time" :max="6" :tick-labels="labels" class="mx-4" ticks></v-slider>
                  <v-card-actions>
                    <v-btn text>Start</v-btn>
                    <v-btn text>Stop</v-btn>
                  </v-card-actions>
                </v-card>
              </v-col>

              <v-col cols="12">
                <v-card color="#952175" dark>
                  <v-card-title>Fuel Pump</v-card-title>
                  <v-slider v-model="time" :max="6" :tick-labels="labels" class="mx-4" ticks></v-slider>
                  <v-card-actions>
                    <v-btn text>Start</v-btn>
                    <v-btn text>Stop</v-btn>
                  </v-card-actions>
                </v-card>
              </v-col>
            </v-row>
          </v-container>
        </v-card>
      </v-col>
    </v-row>

    <v-row class="text-center">
      <v-col cols="12">
        <v-card outlined tile  elevation="10" class="ma-2">
          <v-card-title>Info-Bottom</v-card-title>
        </v-card>
      </v-col>
    </v-row>
  </v-container>
</template>

<script>
  import FanIcon from './FanIcon'
  import GlowPlugIcon from './GlowPlugIcon'
  import FireIcon from './FireIcon'
  import PumpIcon from './PumpIcon'
  import HeaterIcon from './HeaterIcon'
  import { mapGetters } from 'vuex'

  export default {
    name: 'Heater',
    components: {
      FanIcon,
      GlowPlugIcon,
      FireIcon,
      PumpIcon,
      HeaterIcon,
    },
    data: () => ({
      heaterColor: "green",
      labels: ['SU', 'MO', 'TU', 'WED', 'TH', 'FR', 'SA'],
      time: 0,
    }),
    methods: {
    },
    mounted: function () {
      this.statusPollTmr =  setInterval(() => {
        this.$store.dispatch(`pollStatus`)
      }, 1000)
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
      ]),
      flameVisibility() {
        if (this.$store.getters.flameDetected) {
          return 'visible'
        } else {
          return 'hidden'
        }
      },
    },
  }
</script>
