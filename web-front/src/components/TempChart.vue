<template>
  <v-card outlined tile elevation="10">
    <v-card-title>
      <v-row aligh="start">
        <div class="caption grey--text text-uppercase">
          {{ title }}
        </div>
        <div>
          <span class="display-2 font-weight-black" v-text="value.toFixed(1)" />
          <strong>{{ unit }}</strong>
        </div>
      </v-row>
    </v-card-title>

    <v-sheet color="transparent" height="100">
      <v-sparkline
       :smooth="16"
       :gradient="['#f72047', '#ffd200', '#1feaea']"
       :line-width="1"
       :value="trend"
       auto-draw
       stroke-linecap="round"
      >
      </v-sparkline>
    </v-sheet>
  </v-card>
</template>

<script>
  export default {
    props: {
      title: {
        type: String,
        default: '',
      },
      unit: {
        type: String,
        default: '',
      },
      value: {
        type: Number,
        default: 0,
      },
      trend: {
        default: null
      },
    },
  }
</script>
