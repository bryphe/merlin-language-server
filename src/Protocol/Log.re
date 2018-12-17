/* Simple log helper */

let error = (msg: string) => {
  prerr_endline("[ERROR] " ++ msg);
};

let debug = (msg: string) => {
  prerr_endline("[DEBUG] " ++ msg);
};

let verbose = (msg: string) => {
  prerr_endline("[VERBOSE] " ++ msg);
};

let info = (msg: string) => {
  prerr_endline("[INFO] " ++ msg);
};
