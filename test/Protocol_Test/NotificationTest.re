open TestFramework;

module Notification = Protocol.Notification;

[@deriving yojson]
type emptyJson = {noop: string};

[@deriving yojson]
type notificationMessage = {method: string};

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

let createRequestMessage = () => {
  requestMessage_to_yojson({id: 1, method: "test"});
};

describe("Notification", ({describe, _}) =>
  describe("is", ({test, _}) => {
    test("returns false for empty notification", ({expect}) => {
      let emptyJson = createEmptyJson();
      expect.bool(Notification.is(emptyJson)).toBe(false);
    });

    test("returns false for request", ({expect}) => {
      let request = createRequestMessage();
      expect.bool(Notification.is(request)).toBe(false);
    });

    test("returns true for notification", ({expect}) => {
      let notificationJson = createNotificationMessage();
      expect.bool(Notification.is(notificationJson)).toBe(true);
    });
  })
);
