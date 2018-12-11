type documentInfo = {
    uri: Protocol.Types.documentUri,
    text: string,
    version: int,
};

type t = Hashtbl.t<string, documentInfo>;

let create: unit => t = () => Hashtbl.create(16);

let openDocument = (store: t, documentItem)
