<template>
  <v-container>
    <v-row>
      <v-col cols="1">
        <sheet>
          <v-slider
            direction="vertical"
            v-model="radar"
            :max="200"
            :min="0"
            step="0.1"
            thumb-label="always"
            color="green"
            label="Height (in)"
            track-size="50"
          ></v-slider>
        </sheet>
      </v-col>
      <v-col justify="left" cols="11">
        <sheet>
          <figure class="highcharts-figure">
            <div id="container0"></div>
          </figure>
        </sheet>
      </v-col>
    </v-row>
    <v-row>
      <v-col cols="8">
        <sheet>
          <figure class="highcharts-figure">
            <div id="container1"></div>
          </figure>
        </sheet>
      </v-col>
      <v-col cols="4">
        <v-sheet max-width="350px">
          <v-card
            class="mb-5"
            style="max-width: 350px"
            variant="outlined"
            color="primary"
            density="compact"
            rounded="lg"
            title="Water Level"
            subtitle="Capacity Remaining"
          >
            <div id="container2"></div>
          </v-card>
        </v-sheet>
        <v-dialog width="500" v-if="isVariableOver100 == true">
          <template v-slot:activator="{ props }">
            <v-btn
              v-bind="props"
              text="Tank Over Maximum Capacity. Click to view"
            >
            </v-btn> </template
          ><template v-slot:default="{ isActive }">
            <v-card title="Dialog">
              <v-card-text>
                Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do
                eiusmod tempor incididunt ut labore et dolore magna aliqua.
              </v-card-text>

              <v-card-actions>
                <v-spacer></v-spacer>

                <v-btn
                  text="Close Dialog"
                  @click="
                    isActive.value = false;
                    isVariableOver100 = false;
                  "
                ></v-btn>
              </v-card-actions>
            </v-card>
          </template>
        </v-dialog>
      </v-col>
    </v-row>
  </v-container>
</template>

<script setup>
/** JAVASCRIPT HERE */

// IMPORTS

import { useMqttStore } from "@/store/mqttStore"; // Import Mqtt Store
import { storeToRefs } from "pinia";
const Mqtt = useMqttStore();
const { payload, payloadTopic } = storeToRefs(Mqtt);

// Highcharts, Load the exporting module and Initialize exporting module.
import Highcharts from "highcharts";
import more from "highcharts/highcharts-more";
import Exporting from "highcharts/modules/exporting";
Exporting(Highcharts);
more(Highcharts);

import {
  ref,
  reactive,
  watch,
  onMounted,
  onBeforeUnmount,
  computed,
} from "vue";
import { useRoute, useRouter } from "vue-router";

// VARIABLES
const router = useRouter();
const route = useRoute();
var radar = ref(null);
const areaChart = ref(null);
const gauge = ref(null);
var fm = new FluidMeter();
const percentage = ref(null);
const points = ref(600);
const shift = ref(false);
// FUNCTIONS

const CreateCharts = async () => {
  // TEMPERATURE CHART
  areaChart.value = Highcharts.chart("container0", {
    chart: { zoomType: "x" },
    title: { text: "Water Reserves(10 min)", align: "left" },
    yAxis: {
      title: {
        text: "Water Level",
        style: { color: "#000000" },
      },
      labels: { format: "{value} Gal" },
    },
    xAxis: {
      type: "datetime",
      title: { text: "Time", style: { color: "#000000" } },
    },
    tooltip: { shared: true },
    series: [
      {
        name: "Water",
        type: "area",
        data: [],
        turboThreshold: 0,
        color: Highcharts.getOptions().colors[0],
      },
    ],
  });

  gauge.value = Highcharts.chart("container1", {
    title: { text: "Water Reserves", align: "left" },
    // the value axis
    yAxis: {
      min: 0,
      max: 1000,
      tickPixelInterval: 72,
      tickPosition: "inside",
      tickColor: Highcharts.defaultOptions.chart.backgroundColor || "#FFFFFF",
      tickLength: 20,
      tickWidth: 2,
      minorTickInterval: null,
      labels: { distance: 20, style: { fontSize: "14px" } },
      lineWidth: 0,
      plotBands: [
        { from: 0, to: 200, color: "#DF5353", thickness: 20 },
        { from: 200, to: 600, color: "#DDDF0D", thickness: 20 },
        { from: 600, to: 1000, color: "#55BF3B", thickness: 20 },
      ],
    },
    tooltip: { shared: true },
    pane: {
      startAngle: -90,
      endAngle: 89.9,
      background: null,
      center: ["50%", "75%"],
      size: "110%",
    },
    series: [
      {
        type: "gauge",
        name: "Water Capacity",
        data: [0],
        tooltip: { valueSuffix: " Gal" },
        dataLabels: {
          format: "{y} Gal",
          borderWidth: 0,
          color:
            (Highcharts.defaultOptions.title &&
              Highcharts.defaultOptions.title.style &&
              Highcharts.defaultOptions.title.style.color) ||
            "#333333",
          style: { fontSize: "16px" },
        },
        dial: {
          radius: "80%",
          backgroundColor: "gray",
          baseWidth: 12,
          baseLength: "0%",
          rearLength: "0%",
        },
        pivot: { backgroundColor: "gray", radius: 6 },
      },
    ],
  });

  fm.init({
    targetContainer: document.getElementById("container2"),
    fillPercentage: percentage,
    options: {
      fontSize: "70px",
      fontFamily: "Arial",
      fontFillStyle: "white",
      drawShadow: true,
      drawText: true,
      drawPercentageSign: true,
      drawBubbles: true,
      size: 300,
      borderWidth: 25,
      backgroundColor: "#e2e2e2",
      foregroundColor: "#fafafa",
      foregroundFluidLayer: {
        fillStyle: "purple",
        angularSpeed: 100,
        maxAmplitude: 12,
        frequency: 30,
        horizontalSpeed: -150,
      },
      backgroundFluidLayer: {
        fillStyle: "pink",
        angularSpeed: 100,
        maxAmplitude: 9,
        frequency: 30,
        horizontalSpeed: 150,
      },
    },
  });
};

watch(payload, (data) => {
  fm.setPercentage(data.percentage);
  radar.value = data.radar;
  if (points.value > 0) {
    points.value--;
  } else {
    shift.value = true;
  }
  areaChart.value.series[0].addPoint(
    { y: parseFloat(data.reserve.toFixed(2)), x: data.timestamp * 1000 },
    true,
    shift.value
  );
  gauge.value.series[0].points[0].update(parseFloat(data.reserve));

  const isVariableOver100 = computed(() => {
    if (percentage > 100) {
      return true;
    } else {
      return false;
    }
  });
});

onMounted(() => {
  // THIS FUNCTION IS CALLED AFTER THIS COMPONENT HAS BEEN MOUNTED
  Mqtt.connect();
  setTimeout(() => {
    // Subscribe to each topic
    Mqtt.subscribe("620153775");
    Mqtt.subscribe("620153775_sub");
  }, 3000);
  CreateCharts();
});

onBeforeUnmount(() => {
  // THIS FUNCTION IS CALLED RIGHT BEFORE THIS COMPONENT IS UNMOUNTED
  Mqtt.unsubcribeAll();
});
</script>

<style scoped>
/** CSS STYLE HERE */
figure {
  border: 2px solid black;
}
</style>
