/* Preamble
 *
 * This module is for parsing the preamble that comes from language clients, ie:
 *
 * ---
 * Content-Length: 128\r\n
 * Content-Type: ...
 * \r\n
 * ---
 *
 * Relevant spec section: https://microsoft.github.io/language-server-protocol/specification
 */

type t = {
  contentLength: int,
  isComplete: bool,
};

let default: t = {contentLength: (-1), isComplete: false};

let contentLengthHeader = "Content-Length: ";
let contentLengthHeaderLength = String.length(contentLengthHeader);

let endLine = "\r\n";
let endLineLength = String.length(endLine);

let hasContentLengthHeader = s => {
  String.length(s) > contentLengthHeaderLength
  && String.equal(
       String.sub(s, 0, contentLengthHeaderLength),
       contentLengthHeader,
     );
};

let getContentLength = s => {
  let v =
    String.sub(
      s,
      contentLengthHeaderLength,
      String.length(s) - endLineLength - contentLengthHeaderLength,
    );
  int_of_string(v);
};

let parseLine = (~current: t=default, s: string) =>
  if (String.equal(s, "\r\n")) {
    let ret: t = {...current, isComplete: true};
    ret;
  } else if (hasContentLengthHeader(s)) {
    let contentLength = getContentLength(s);
    let ret: t = {...current, contentLength};
    ret;
  } else {
    current;
  };

let read = (ic: in_channel) => {
  let currentParseState = ref(default);

  while (currentParseState^.isComplete == false) {
    let line = input_line(ic);
    currentParseState := parseLine(~current=currentParseState^, line ++ "\n");
  };

  currentParseState^;
};
