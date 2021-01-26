<template>
  <v-dialog
   :value="open"
   transition="dialog-bottom-transition"
   max-width="600"
   @click:outside="$emit('close')"
   @keydown.esc="$emit('close')"
  >
    <v-card>
      <v-toolbar color="primary" dark>
        Fan
      </v-toolbar>
      <v-card-text>
        <v-row
         class="mb-4"
         justify="space-between"
        >
          <v-col class="text-left">
            <span class="display-3 font-weight-light" v-text="fanPower"/>
            <span class="subheading font-weight-light mr-1">%</span>
          </v-col>
          <v-col class="text-right">
            <span class="display-3 font-weight-light">
              {{ fanRunning ? 'Running' : 'Off' }}
            </span>
          </v-col>
        </v-row>

        <v-slider
         thumb-label="always"
         step="1"
         min="0"
         max="100"
         v-model="newFanPower"
         @end="$emit('power', newFanPower)"
        >
        </v-slider>
      </v-card-text>
      <v-card-actions class="justify-end">
        <v-btn :disabled="fanRunning" text @click="$emit('start')">Start</v-btn>
        <v-btn :disabled="!fanRunning" text @click="$emit('stop')">Stop</v-btn>
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
      ]),
    },
    props: {
      open: {
        type: Boolean,
        default: false,
      },
    },
    methods: {
      onStartClick() {
      },
      onStopClick() {
      },
    },
    data: () => ({
      newFanPower: 0,
    }),
    watch: {
      open: function(newVal) {
        if (newVal) {
          this.newFanPower = this.fanPower
        }
      }
    }
  }
</script>

