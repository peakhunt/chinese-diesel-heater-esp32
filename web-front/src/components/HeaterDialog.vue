<template>
  <v-dialog
   :value="open"
   transition="dialog-bottom-transition"
   max-width="600"
   @click:outside="$emit('close')"
  >
    <v-card>
      <v-toolbar color="primary" dark>
        Heater
      </v-toolbar>

      <v-card-text>
        <v-row
         class="mb-4"
         justify="space-between"
        >
          <v-col class="text-right">
            <span class="display-2 font-weight-light">
              {{ fanRunning ? 'Fan running at ' + fanPower + '%' : 'Fan stopped' }}
            </span>
          </v-col>
        </v-row>

        <v-row
         class="mb-4"
         justify="space-between"
        >
          <v-col class="text-right">
            <span class="display-2 font-weight-light">
              {{ glowPlugOn ? 'Glow Plug On ' : 'Glow Plug Off' }}
            </span>
          </v-col>
        </v-row>

        <v-row
         class="mb-4"
         justify="space-between"
        >
          <v-col class="text-right">
            <span class="display-2 font-weight-light">
              {{ pumpRunning ? 'Pump running at ' + pumpFreq.toFixed(1) + 'Hz' : 'Pump stopped' }}
            </span>
          </v-col>
        </v-row>
      </v-card-text>

      <v-card-actions class="justify-end">
        <v-btn text @click="$emit('start')">Start</v-btn>
        <v-btn text @click="$emit('stop')">Stop</v-btn>
      </v-card-actions>
    </v-card>
  </v-dialog>
</template>

<script>
  import { mapGetters } from 'vuex'

  export default {
    computed: {
      ...mapGetters([
        'fanRunning',
        'fanPower',
        'glowPlugOn',
        'pumpRunning',
        'pumpFreq',
      ]),
    },
    props: {
      open: {
        type: Boolean,
        default: false,
      },
    },
  }
</script>

