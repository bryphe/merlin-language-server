type documentInfo = {
    uri: Protocol.Types.documentUri,
    text: string,
    version: int,
};

type t = Hashtbl.t(string, documentInfo);

let create: unit => t = () => Hashtbl.create(16);

let openDocument = (store: t, documentItem: Protocol.Types.textDocumentItem) => {
    Hashtbl.remove(store, documentItem.uri);
    Hashtbl.add(store, documentItem.uri, {
        uri: documentItem.uri,
        text: documentItem.text,
        version: 0,
    });
};

let getDocument = (store: t, uri: Protocol.Types.documentUri) => {
   Hashtbl.find_opt(store, uri); 
};
