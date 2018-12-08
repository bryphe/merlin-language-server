open TestFramework;

open Lib;

[@deriving yojson]
type emptyJson = {
    noop: string
};

[@deriving yojson]
type notificationMessage = {
    method: string
};

[@deriving yojson]
type requestMessage = {
    id: int,
    method: string,
};

let createEmptyJson = () => {
    emptyJson_to_yojson({noop: ""});
};

let createNotificationMessage = () => {
    notificationMessage_to_yojson({method: "test"});
};

let createRequestMessage =() => {
    requestMessage_to_yojson({id: 1, method: "test"});
};


describe("Protocol", ({test, describe}) => {

    describe("isNotification", ({test, _}) => {
        test("returns false for empty notification", ({expect}) => {
            let emptyJson = createEmptyJson();
            expect.bool(Protocol.isNotification(emptyJson)).toBe(false);
        });

        test("returns false for request", ({expect}) => {
            let request = createRequestMessage();
            expect.bool(Protocol.isNotification(request)).toBe(false);
        });

        test("returns true for notification", ({expect}) => {
            let notificationJson = createNotificationMessage();
            expect.bool(Protocol.isNotification(notificationJson)).toBe(true);
        })
    });

    describe("isRequest", ({test, _}) => {
        test("returns false for empty json", ({expect}) => {
            let emptyJson = createEmptyJson();
            expect.bool(Protocol.isRequest(emptyJson)).toBe(false);
        });

        test("returns false for notification", ({expect}) => {
            let notification = createNotificationMessage();
            expect.bool(Protocol.isRequest(notification)).toBe(false);
        });

        test("returns true for request", ({expect}) => {
            let request = createRequestMessage();
            expect.bool(Protocol.isRequest(request)).toBe(true);
        });
    })

    test("Hello world", ({expect}) => {
        expect.int(1+1).toBe(2);
    });
})
