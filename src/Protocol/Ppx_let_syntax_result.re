   let bind  = (~f, v) => {
    switch(v) {
    | Ok(v) => f(v)
    | Error(e) => Error(e)
    };
   };

let map = (~f) => fun
| Ok(v) => Ok(f(v))
| Error(e) => Error(e)

module Let_syntax {
    let bind = bind;
    let map = map;
}
