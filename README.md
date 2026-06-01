Based on the original Timely watchface by Martin Norland cyn@cyn.org, original source: https://github.com/cynorg/PebbleTimely.

Also based on the updates made by Alan Johnson https://github.com/alan-johnson/PebbleTimely

I have kept the original spirit of the orginal with a few twists.

1.) I have added the ability to sync the phone battery life as a option. If disabled, it will show the standard link and unlink like before. If the phone battery watcher is enabled, it will replace it with the phone battery percentage and put a bluetooth icon in the center indicating connection status.

2.) I also have updated the weather system. The old app would sometimes not be able to refresh the weather and would time out on loading the watchface. I have made some adjustments to help prevent this.

3.) Updated to allow internals for how often to grab weather and the battery life of the phone.

4.) Reduced /optimize the memory footprint and battery drain of the watch face as best as possible.

Currently it only supports Pebble Time 2. I may support more if there is a demand for it. But it can get to be tricky because of memory.
