<template>
  <v-container>
    <v-row class="text-center">
      <v-col
        class="mb-5"
        cols="12"
      >
        <h2 class="headline font-weight-bold mb-3">
          Fan
        </h2>

        <v-row justify="center">
          <FanIcon width="200px" height="200px" iconColor="red" />
          <FanIcon width="200px" height="200px" iconColor="blue" />
        </v-row>
      </v-col>

      <v-col
        class="mb-5"
        cols="12"
      >
        <h2 class="headline font-weight-bold mb-3">
          Plug
        </h2>

        <v-row justify="center">
          <GlowPlugIcon width="200px" height="200px" offColor="black" onColor="red"/>
          <GlowPlugIcon width="200px" height="200px" offColor="black" onColor="blue"/>
        </v-row>
      </v-col>

      <v-col
        class="mb-5"
        cols="12"
      >
        <h2 class="headline font-weight-bold mb-3">
          Fire
        </h2>

        <v-row justify="center">
          <FireIcon visibility="visible" width="200px" height="200px" iconColor="red"/>
        </v-row>
      </v-col>

      <v-col
        class="mb-5"
        cols="12"
      >
        <h2 class="headline font-weight-bold mb-3">
          Pump
        </h2>

        <v-row justify="center">
          <PumpIcon iconColor="green" width="200px" height="200px"/>
        </v-row>
      </v-col>

      <v-col
        class="mb-5"
        cols="12"
      >
        <h2 class="headline font-weight-bold mb-3">
          Heater
        </h2>

        <v-row justify="center">
          <HeaterIcon iconColor="green" width="900px" height="600px"/>
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
    }),
  }
</script>
