<template>
<svg
  xmlns="http://www.w3.org/2000/svg"
  :width="width"
  :height="height"
  viewBox="0 0 88 94.963837"
  role="presentation">

  <g :fill="pumpColor" transform="rotate(-90 44 47.4819185)">
    <path
     d="M 15.8,5.7 C 15.5,9.5 14,14.2 11.7,19.2 L 8.1,27 h -4 C 0.4,27 0,27.2 0,29.5 0,31.9 0.3,32 5.8,32 h 5.8 L 16.3,21.1 C 19.9,12.8 21,8.9 21,5.1 21,0.4 20.8,0 18.6,0 16.4,0 16.2,0.5 15.8,5.7 Z"/>
    <path
     d="M 30,17.5 V 35 h 8.5 C 45.4,35 47,34.7 47,33.5 47,32.2 49.2,32 61,32 H 75 V 46 60 H 61 C 49.2,60 47,59.8 47,58.5 47,57.3 45.4,57 38.5,57 H 30 L 29.8,73.2 29.5,89.5 H 27 c -2.2,0 -3.1,-1.4 -8.9,-14.8 L 11.6,60 H 5.8 C 0.3,60 0,60.1 0,62.5 0,64.8 0.4,65 4.3,65 h 4.2 l 5.4,12.8 c 2.9,7 6.5,13.7 7.8,14.9 3.2,3.1 7.7,3 10.8,-0.2 2.4,-2.3 2.5,-3 2.5,-15 V 65 H 57.5 80 V 46 27 H 57.5 35 V 13.5 0 H 32.5 30 Z"/>
    <path
     d="m 48,21.5 c 0,2.2 0.4,2.5 3.5,2.5 1.9,0 3.5,0.4 3.5,1 0,0.6 3.8,1 9.5,1 5.7,0 9.5,-0.4 9.5,-1 0,-0.6 2.3,-1 5,-1 h 5 V 45.6 67.2 L 73.5,66.5 c -5.8,-0.4 -13.9,-0.4 -18,0 -7.1,0.6 -7.5,0.8 -7.5,3.1 V 72 H 68 88 V 45.5 19 H 68 48 Z"/>
    <path ref="cylinder"
     d="m 19.1,34.1 c -2.9,3.2 -4.4,8.4 -3.9,13.9 0.5,5.1 4,11 6.4,11 2.5,0 2.7,-2.8 0.5,-5.6 C 18,48.2 18.4,48 37.5,48 H 55 v 5 5 H 64.5 74 V 45.5 33 H 64.5 55 V 38.5 44 H 37.5 C 18.3,44 18.4,44.1 22.1,38.1 23.9,35.1 24,34.5 22.7,33.3 21.4,32 21,32.1 19.1,34.1 Z"/>
    <rect x="0" y="0" width="88" height="94.963837" style="fill-opacity: 0" @click="$emit('click')"/>
  </g>
</svg>
</template>

<script>
  import { TweenMax } from 'gsap'

  export default {
    name: 'PumpIcon',
    props: {
      width: {
        type: [Number, String],
        default: 18
      },
      height: {
        type: [Number, String],
        default: 18
      },
      onColor: {
        type: String,
        default: 'blue'
      },
      offColor: {
        type: String,
        default: 'black'
      },
      running: {
        type: Boolean,
        default: false
      },
      iconColor: {
        type: String,
        default: 'currentColor'
      }
    },
    computed: {
      pumpColor: function() {
        if (this.running) {
          return this.onColor
        } else {
          return this.offColor
        }
      }
    },
    mounted: function () {
      this.pumpAnim = TweenMax.to(this.$refs.cylinder, 0.25, {
        x: -7,
        repeat: -1,
        ease: 'none',
        paused: !this.running,
        yoyo: true,
      });
    },
    methods: {
      startPump() {
        if (this.pumpAnim.paused() == false) {
          this.pumpAnim.pause(0)
        } else {
          this.pumpAnim.play()
        }
      },
    },
    data: () => ({
      pumpAnim: null,
    }),
    watch: {
      running: function(newVal) {
        if (newVal) {
          this.pumpAnim.play()
        } else {
          this.pumpAnim.pause(0)
        }
      }
    }
  }
</script>
