<template>
  <v-dialog
   :value="value"
   @input="$emit('input', $event)"
   transition="dialog-bottom-transition"
   max-width="600"
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
        >
        </v-slider>
      </v-card-text>
      <v-card-actions class="justify-end">
        <v-btn text @click="$emit('power', newFanPower)">Update</v-btn>
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
      value: {
        type: Boolean,
        default: false,
      },
    },
    methods: {
    },
    data: () => ({
      newFanPower: 0,
    }),
    watch: {
      value: function(newVal) {
        if (newVal) {
          this.newFanPower = this.fanPower
        }
      }
    }
  }
</script>

