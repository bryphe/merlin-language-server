module Log = Protocol.Log;

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

let getDocumentLine =
    (
      store: t,
      uri: Protocol.Types.documentUri,
      line: Protocol.Types.zeroBasedLine,
    ) => {
  switch (getDocument(store, uri)) {
  | None => None
  | Some(v) =>
    let lines = v.text |> String.split_on_char('\n') |> Array.of_list;

    line < Array.length(lines) ? Some(lines[line]) : None;
  };
};

let getTokenAt =
    (
      store: t,
      uri: Protocol.Types.documentUri,
      position: Protocol.Types.Position.t,
    ) => {
  let line = getDocumentLine(store, uri, position.line);

  switch (line) {
  | None => None
  | Some(l) =>
    /* Get first space prior to token */
    Log.debug("DocumentStore::getTokenAt: |" ++ l ++ "| character: " ++ string_of_int(position.character));
    let firstPriorSpace =
      switch (String.rindex_from_opt(l, position.character, ' ')) {
      | v => v
      | exception (Invalid_argument(_)) => None
      };
    let startPos =
      switch (firstPriorSpace) {
      | Some(x) => x
      | None => 0
      };
    Log.debug(
      "DocumentStore::getTokenAt: "
      ++ l
      ++ " start pos: "
      ++ string_of_int(startPos),
    );
    Some(String.sub(l, startPos, position.character - startPos));
  };
};
