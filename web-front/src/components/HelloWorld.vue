<template>
  <v-container>
    <v-row class="text-center">
      <v-col
        class="mb-5"
        cols="12"
      >
        <v-row justify="center">
          <div style="position: relative" width="1280px" height="768px">
            <HeaterIcon
             style="position: absolute; top: 0px; left:-450px"
             :iconColor="heaterColor"
             width="900px"
             height="600px"/>

            <FanIcon
             style="position: absolute; top: 220px; left: 250px"
             width="150px"
             height="150px"
             onColor="blue"
             offColor="black"
             :running="this.$store.getters.fanRunning" />

            <GlowPlugIcon
             style="position: absolute; top: 150px; left: 10px"
             width="150px"
             height="150px"
             offColor="black"
             onColor="red"
             :running="this.$store.getters.glowPlugOn"/>

            <FireIcon
             style="position: absolute; top: 150px; left: -250px"
             :visibility="flameVisibility"
             width="300px"
             height="300px"
             :running="this.$store.getters.flameDetected"
             />
            <PumpIcon
             style="position: absolute; top: 50px; left: 400px"
             width="150px"
             height="150px"
             onColor="blue"
             offColor="black"
             :running="this.$store.getters.pumpRunning" />

            <svg style="position: absolute; top: 280px; left: -420px" width="130px" height="40px">
              <line x1="0" y1="0" x2="130" y2="0" style="stroke:black; stroke-width: 76"/>  
              <text x="5" y="30" style="font: bold 30px sans-serif;fill: red;">{{this.$store.getters.outletTemp}} °C</text>
            </svg>

            <svg style="position: absolute; top: 380px; left: 260px" width="90px" height="40px">
              <line x1="0" y1="0" x2="90" y2="0" style="stroke:black; stroke-width: 76"/>  
              <text x="5" y="30" style="font: bold 30px sans-serif;fill: red;">{{this.$store.getters.fanPower}}%</text>
            </svg>

            <svg style="position: absolute; top: 70px; left: 320px" width="100px" height="40px">
              <line x1="0" y1="0" x2="100" y2="0" style="stroke:black; stroke-width: 76"/>  
              <text x="5" y="30" style="font: bold 30px sans-serif;fill: red;">{{this.$store.getters.pumpFreq}} Hz</text>
            </svg>

          </div>
        </v-row>
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

  export default {
    name: 'HelloWorld',
    components: {
      FanIcon,
      GlowPlugIcon,
      FireIcon,
      PumpIcon,
      HeaterIcon,
    },
    data: () => ({
      heaterColor: "green",
    }),
    computed: {
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
