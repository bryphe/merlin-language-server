open TestFramework;

module DocumentStore = Server.DocumentStore;

describe("DocumentStore", ({test, _}) => {
  test("gets item that was set", ({expect}) => {
    let store = DocumentStore.create();

    DocumentStore.openDocument(store, {
        uri: "file:///test-document.txt",
        languageId: "txt",
        text: "Hello\nWorld\n",
    })

    let result = DocumentStore.getDocument(store, "file:///test-document.txt");

    switch (result) {
    | None => expect.bool(false).toBe(true);
    | Some(x) => expect.string(x.text).toEqual("Hello\nWorld\n");
    }
  });

  test("returns none if document isn't set", ({expect}) => {
    let store = DocumentStore.create();

    let result = DocumentStore.getDocument(store, "file:///some-non-existent-file.txt");

    switch (result) {
    | None => expect.bool(true).toBe(true);
    | Some(_) => expect.bool(false).toBe(true);
    }
  });
});
