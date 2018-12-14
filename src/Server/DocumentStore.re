type documentInfo = {
  uri: Protocol.Types.documentUri,
  text: string,
  version: int,
};

type t = Hashtbl.t(string, documentInfo);

let create: unit => t = () => Hashtbl.create(16);

let openDocument =
    (store: t, documentItem: Protocol.Notification.didOpenTextDocumentParams) => {
  let uri = documentItem.textDocument.uri;
  let text = documentItem.textDocument.text;
  Hashtbl.remove(store, uri);
  Hashtbl.add(store, uri, {uri, text, version: 0});
};


/*
 * Because we only implement full document syncing right now, this is basic,
 * but once we have incremental syncing, there will be more interesting logic here.
 */
let changeDocument =
    (store: t, change: Protocol.Notification.didChangeTextDocumentParams) => {
  let text = List.hd(change.contentChanges).text;
  let uri = change.textDocument.uri;

  Hashtbl.remove(store, uri);
  Hashtbl.add(
    store,
    uri,
    {
      uri,
      text,
      version: 0 /* TODO */
    },
  );
};

let getDocument = (store: t, uri: Protocol.Types.documentUri) => {
  Hashtbl.find_opt(store, uri);
};

let getDocumentLine = (store: t, uri: Protocol.Types.documentUri, line: Protocol.Types.zeroBasedLine) => {
    switch (getDocument(store, uri)) {
    | None => None
    | Some(v) => {
            let lines = v.text
                |> String.split_on_char('\n')
                |> Array.of_list;

            switch (line < Array.length(lines)) {
            | true => Some(lines[line])
            | false => None
            };
        };
    }
};
