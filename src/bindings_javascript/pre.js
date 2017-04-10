/** Copyright 2017 ButterflyJS */

var Module = {
  'locateFile': function(text) {
    // check for function
    if (typeof g_LocateFile == 'function') {
      console.log("[bf] g_LocateFile")
      return g_LocateFile(text);
    }

    if (typeof(window) != "undefined") {
      if (window.locatefile) {
        console.log("[bf] w.locatefile")
        return window.locatefile(text);
      } else {
        console.log("[bf] return")
        return text;
      }
    }

    return text;
  },
  'onRuntimeInitialized': function() {
    // check for function
    if (typeof g_OnASMReady == 'function') {
      console.log("[bf] g_OnASMReady");
      g_OnASMReady();
      return 0;
    }

    if (typeof(window) != "undefined") {
      if (window.onASMReady) {
        console.log("[bf] w.onASMReady");
        window.onASMReady();
      }
    }
  }
};
