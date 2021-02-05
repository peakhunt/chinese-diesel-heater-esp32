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
       @click="$refs.heater.openCommPortDialog()"
       >
       {{ statusMsg }}
      </v-chip>

      <v-chip
       color="success"
       elevation="4"
       @click="$refs.heater.openSettingsDialog()"
       >
        Settings
      </v-chip>
    </v-app-bar>

    <v-main>
      <Heater ref="heater"/>
    </v-main>
  </v-app>
</template>

<script>
import Heater from './components/Heater'
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
/// #if RUN_TARGET == 'electron'
      'portName',
/// #endif
    ]),
    statusMsg() {
      /*eslint no-unreachable: "off"*/
/// #if RUN_TARGET == 'electron'
      if (this.portName !== '') {
        return this.commStatus ? 'Comm OK' : 'Comm Fail'
      }
      return 'Not Open'
/// #elif RUN_TARGET == 'web'
      return this.commStatus ? 'Comm OK' : 'Comm Fail'
/// #endif
    },
  }
};
</script>

<style lang="sass">
  @import '../node_modules/typeface-roboto/index.css'
</style>
