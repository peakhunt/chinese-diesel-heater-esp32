<template>
  <v-dialog
   :value="value"
   @input="$emit('input', $event)"
   transition="dialog-bottom-transition"
   max-width="600"
  >
    <v-card>
      <v-toolbar color="primary" dark>
        Fuel Pump
      </v-toolbar>

      <v-card-text>
        <v-row
         class="mb-4"
         justify="space-between"
        >
          <v-col class="text-left">
            <span class="display-3 font-weight-light">{{pumpFreq.toFixed(1)}}</span>
            <span class="subheading font-weight-light mr-1">Hz</span>
          </v-col>
          <v-col class="text-right">
            <span class="display-3 font-weight-light">
              {{ pumpRunning ? 'Running' : 'Off' }}
            </span>
          </v-col>
        </v-row>

        <v-slider
         thumb-label="always"
         step="0.1"
         min="0.5"
         max="5.0"
         v-model="newPumpFreq"
        >
        </v-slider>
      </v-card-text>

      <v-card-actions class="justify-end">
        <v-btn text @click="$emit('freq', newPumpFreq)">Update</v-btn>
        <v-btn :disabled="pumpRunning" text @click="$emit('start')">Start</v-btn>
        <v-btn :disabled="!pumpRunning" text @click="$emit('stop')">Stop</v-btn>
      </v-card-actions>
    </v-card>
  </v-dialog>
</template>

<script>
  import { mapGetters } from 'vuex'

  export default {
    computed: {
      ...mapGetters([
        'pumpRunning',
        'pumpFreq',
      ]),
    },
    props: {
      value: {
        type: Boolean,
        default: false,
      },
    },
    data: () => ({
      newPumpFreq: 0,
    }),
    watch: {
      value: function(newVal) {
        if (newVal) {
          this.newPumpFreq = this.pumpFreq
        }
      }
    }
  }
</script>

