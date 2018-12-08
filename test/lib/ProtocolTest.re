open TestFramework;

describe("Protocol", ({test, _}) => {
    test("Hello world", ({expect}) => {
        expect.int(1+1).toBe(2);
    });
})
