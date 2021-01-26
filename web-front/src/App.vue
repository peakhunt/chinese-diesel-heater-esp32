<template>
  <v-app>
    <v-app-bar
      app
      dense
      color="primary"
      dark
    >
      <v-toolbar-title>
        Chinese Diesel Heater
      </v-toolbar-title>

      <v-spacer></v-spacer>

      <v-chip
       :color="commStatus ? 'success' : 'error'"
       elevation="4"
       >
        Comm {{ commStatus ? 'OK' : 'Fail' }}
      </v-chip>
    </v-app-bar>

    <v-main>
      <Heater/>
    </v-main>
  </v-app>
</template>

<script>
import Heater from './components/Heater';
import { mapGetters } from 'vuex'

export default {

  name: 'App',
  components: {
    Heater,
  },
  data: () => ({
  }),
  computed: {
    ...mapGetters([
      'commStatus',
    ]),
  }
};
</script>

<style lang="sass">
  @import '../node_modules/typeface-roboto/index.css'
</style>
