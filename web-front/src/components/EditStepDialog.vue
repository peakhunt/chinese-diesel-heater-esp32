<template>
  <v-dialog
   :value="value"
   @input="$emit('input', $event)"
   width="600"
  >
    <v-card v-if="desc !== null">
      <v-card-title>step {{desc.index}}</v-card-title>
      <v-card-text>
        <v-row>
          <v-col cols="2">Fan Power</v-col>
          <v-col>
            <v-slider
             thumb-label="always"
             step="1"
             min="10"
             max="100"
             v-model="power"
            />
          </v-col>
        </v-row>

        <v-row>
          <v-col cols="2">Pump Frequency</v-col>
          <v-col>
            <v-slider
             thumb-label="always"
             step="0.1"
             min="0.8"
             max="5.0"
             v-model="freq"
            />
          </v-col>
        </v-row>
      </v-card-text>
      <v-divider></v-divider>
      <v-card-actions>
        <v-spacer/>
        <v-btn color="primary" text @click="$emit('change', { desc, power, freq })">Update</v-btn>
      </v-card-actions>
    </v-card>
  </v-dialog>
</template>

<script>
  export default {
    data: () => ({
      power: 0,
      freq: 0.0,
    }),
    props: {
      value: {
        type: Boolean,
        default: false,
      },
      desc: {
        type: Object,
        default: null
      },
    },
    methods: {
    },
    watch: {
      value: function(newVal) {
        if (newVal) {
          this.power = this.desc.step.fan_pwr
          this.freq = this.desc.step.pump_freq
        }
      },
    },
  }
</script>
