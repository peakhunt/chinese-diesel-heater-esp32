<template>
  <v-app>
    <v-app-bar
      app
      color="primary"
      dark
    >
      <div class="d-flex align-center">
        Chinese Diesel Heater
      </div>

      <v-spacer></v-spacer>

      <v-btn class="mx-2" dark large :color="commStatus ? 'green' : 'red'">
        Comm {{ commStatus ? 'OK' : 'Fail' }}
      </v-btn>
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
