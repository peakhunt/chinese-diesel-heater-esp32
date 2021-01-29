<template>
  <v-dialog
   :value="value"
   @input="$emit('input', $event)"
   width="400"
  >
    <v-card v-if="desc !== null">
      <v-card-title>{{desc.name}}</v-card-title>
      <v-card-text>
        <br>
        <v-slider
         thumb-label="always"
         :step="desc.step"
         :min="desc.min"
         :max="desc.max"
         v-model="setValue"
        />
      </v-card-text>
      <v-divider></v-divider>
      <v-card-actions>
        <v-spacer/>
        <v-btn color="primary" text @click="$emit('change', { desc, setValue })">Update</v-btn>
      </v-card-actions>
    </v-card>
  </v-dialog>
</template>

<script>
  export default {
    data: () => ({
      setValue: 0,
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
      setting: {
        type: Number,
        default: 0
      },
    },
    methods: {
    },
    watch: {
      value: function(newVal) {
        if (newVal) {
          this.setValue = this.setting
        }
      },
    },
  }
</script>
