open Tree;
open Grammar;
let parse = (tokens, weight: operator => int) => {
  let rest = ref(tokens);

  let rec loop = current_tree => {
    let (token, tail) =
      switch (rest^) {
      | [t, ...ts] => (Some(t), ts)
      | [] => (None, [])
      };
    rest := tail;

    switch (token, current_tree) {
    // 4
    | (None, ct) => ct
    // 0
    | (Some(Number(a)), ct) => loop(ct->Tree.insert(Number(a)))
    // 0 var
    | (Some(Variable(a)), ct) => loop(ct->Tree.insert(Variable(a)))
    // 1
    | (Some(Operator(Infix(op))), N0(old_val)) =>
      loop(N2(Infix(op), old_val, Empty))
    /* 2-legacy *
        | (Some(Operator(Infix(_) as op)), N2(current_op, a, b))
          when weight(op) > weight(current_op) =>
        loop(N2(current_op, a, loop(N2(op, b, Empty))))
       */
    // 2
    | (
        Some(Operator(Infix(_) as op)),
        (N2(current_op, _, old_val) | N1(current_op, old_val)) as ct,
      )
        when weight(op) > weight(current_op) =>
      loop(ct->Tree.insert(loop(N2(op, old_val, Empty))))
    /* 6-legacy
     * | (Some(Operator(Infix(_) as op)), N2(current_op, _, _) as old_val)
        when weight(op) <= weight(current_op) =>
      loop(N2(op, old_val, Empty))
     */
    // 6
    | (
        Some(Operator(Infix(_) as op)),
        (N2(current_op, _, _) | N1(current_op, _)) as old_val,
      )
        when weight(op) <= weight(current_op) =>
      loop(N2(op, old_val, Empty))
    // 3.1
    | (Some(Bracket(Open)), ct) =>
      loop(ct->Tree.insert(loop(N0(Empty))))
    // 3.2
    | (Some(Bracket(Close)), ct) => ct
    // 8
    | (Some(Operator(Suffix(_) as op)), ct)
        when weight_of_node(ct) <= weight_of_node(N1(op, N0(Empty))) =>
      loop(N1(op, ct))
    // 5
    | (
        Some(Operator(Suffix(_) as op)),
        (N2(_, _, old_val) | N1(_, old_val) | N0(old_val)) as ct,
      )
        when weight_of_node(ct) > weight_of_node(N1(op, Empty)) =>
      loop(ct->Tree.insert(N1(op, old_val)))
    // 7
    | (Some(Operator(Function(_) as f)), ct) =>
      loop(ct->Tree.insert(loop(N1(f, Empty))))
    | (a, b) =>
      [%debugger];
      raise(Js.Exn.raiseError({j|Unmatched state ( $a ; $b )|j}));
    };
  };
  loop(N0(Empty));
};

/**Finds the variables of a function expression
 * ```reason
 * ("f(a, b) = ....") == ["a", "b"]
 * ```
 */;
/*
 let variables_of_fx = tokens => {
   let (_, vs) =
     tokens->Belt.List.reduce((false, []), ((done_searching, acc), token) =>
       switch (done_searching, token) {
       | (false, Variable(v)) => (false, [v, ...acc])
       | (false, Bracket(Close)) => (true, acc)
       | _ => (done_searching, acc)
       }
     );

   vs->List.rev;
 };*/