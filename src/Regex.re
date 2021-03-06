open Js.Re;
let re_num = [%bs.re "/[\d\.]+/"];
let re_name = [%bs.re "/[a-zA-Z]/i"];
let re_space_any = [%bs.re "/\s*/"];
let re_sign = [%bs.re "/[\+\-]/"];
let re_eol = Js.Re.fromString("\n+$");

let is_num = test_(re_num);
let is_name = test_(re_name);
let is_sign = test_(re_sign);

let escape_regex = str =>
  str->Js.String2.replaceByRe([%bs.re "/[.*+?^${}()|[\]\\-]/g"], "\\$&");

module Make = {
  let is_op = operators => {
    let re =
      (
        "["
        ++ operators
           ->Js.String2.replaceByRe([%bs.re "/\s+/gi"], "")
           ->escape_regex
        ++ "]"
      )
      ->fromString;

    test_(re);
  };

  let is_function = (functions: array(string)) => {
    let re =
      "("
      ++ (
        functions->Belt.Array.map(f => f->escape_regex)
        |> Js.Array.joinWith("|")
      )
      ++ ")"
      |> fromString;
    test_(re);
  };
};

/**matches: (name, variable, expression) */
let re_function_exp = [%bs.re "/(?<name>\w+)\((?<var>\w)\)\s*=\s*(?<exp>.*)/"];