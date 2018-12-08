/* let () = Lwt_main.run(Util.hello()); */


set_binary_mode_in(stdin, true);
set_binary_mode_out(stdout, true);

let id = Unix.descr_of_in_channel(stdin);

/* let ic = Unix.in_channel_of_descr(stdin); */

 [@deriving yojson {strict: false} ]
type notification = {
    jsonrpc: string,   
    method: string,
}; 


let shouldClose = ref(false);

while (!shouldClose^) {
	Thread.wait_read(id);
	let contentLength = input_line(stdin);
	/* let contentType = input_line(stdin); */

	let preambleLength = String.length("Content-Length: ");
    /* let postLength = String.length("\r\n"); */
	/* let getContentLength = String.sub(contentLength, preambleLength, String.length(contentLength) - postLength - preambleLength + 1); */
	let getContentLength = String.sub(contentLength, preambleLength, 2);
    prerr_endline( "|" ++ getContentLength ++ "|");
 	let len = int_of_string(getContentLength);

	let _ = input_line(stdin);

    /* Read message */

	let buffer = Bytes.create(len);
	let _ = input(stdin, buffer, 0, len);

    let str = Bytes.to_string(buffer);

    let json = Yojson.Safe.from_string(str);
    let n = notification_of_yojson(json);


    switch (n) {
    | Ok(v) => 
            
        if (String.equal(v.method, "exit")) {
            prerr_endline ("EXIT RECEIVED");
           shouldClose := true; 
        } else {
            prerr_endline ("EXIT NOT RECEIVED: " ++ v.method);
        }
    | Error(_) => prerr_endline ("Parse error: " ++ str);
    }
}

prerr_endline ("Closing");
