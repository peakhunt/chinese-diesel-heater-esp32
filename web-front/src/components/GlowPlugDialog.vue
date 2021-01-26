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
        Glow Plug
      </v-toolbar>

      <v-card-text>
        <v-row
         class="mb-4"
         justify="space-between"
        >
          <v-col class="text-left">
          </v-col>
          <v-col class="text-right">
            <span class="display-3 font-weight-light">
              {{ glowPlugOn ? 'On' : 'Off' }}
            </span>
          </v-col>
        </v-row>
      </v-card-text>

      <v-card-actions class="justify-end">
        <v-btn :disabled="glowPlugOn" text @click="$emit('on')">Turn On</v-btn>
        <v-btn :disabled="!glowPlugOn" text @click="$emit('off')">Turn Off</v-btn>
      </v-card-actions>
    </v-card>
  </v-dialog>
</template>

<script>
  import { mapGetters } from 'vuex'

  export default {
    computed: {
      ...mapGetters([
        'glowPlugOn',
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
