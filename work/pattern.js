var sys = require("sys");

function puts(x){
    sys.puts(x);
}

function variable(name){
    return { tag: "VAR", name: name };
}

function integer(val){
    return { tag: "INT", val: val };
}

function bool(val){
    return { tag: "BOOL", val: val };
}

function string(val){
    return { tag: "STRING", val: val };
}

function tuple(vals){
    return { tag: "TUPLE", length: vals.length, vals: vals };
}

function variant(cons, vals){
    return { tag: "VARIANT", cons: cons, length: vals.length, vals: vals };
}

function show_item(item){
    switch(item.tag){
        case "VAR":{
            //return "VAR(" + item.name + ")";
            return item.name;
        }
        case "INT":{
            //return "INT(" + item.val + ")";
            return item.val;
        }
        case "BOOL":{
            //return "BOOL(" + item.val + ")";
            return (item.val) ? "True" : "False";
        }
        case "STRING":{
            //return "STRING(" + item.val + ")";
            return "\"" + item.val + "\"";
        }
        case "TUPLE":{
            //return "TUPLE(" + item.vals.map(show_item).join(", ") + ")";
            return "(" + item.vals.map(show_item).join(", ") + ")";
        }
        case "VARIANT":{
            //return item.cons + "(" + item.vals.map(show_item).join(", ") + ")";
            return item.cons + " " + item.vals.map(show_item).join(" ");
        }
    }
}

function equal_item(i1, i2){
    if(i1.tag != i2.tag){
        return false;
    }

    switch(i1.tag){
        case "VAR":
        {
            return i1.name == i2.name;
        }
        case "INT":
        case "BOOL":
        case "STRING":
        {
            return i1.val == i2.val;
        }
        case "TUPLE":
        {
            var ret = true;
            i1.vals.map(function(e, i){
                    ret &= equal_item(e, i2.vals[i]);
            });
            return ret;
        }
        case "VARIANT":
        {
            var ret = i1.cons == i2.cons;
            i1.vals.map(function(e, i){
                    ret &= equal_item(e, i2.vals[i]);
            });
            return ret;
        }
    }

    return false;
}

//
// i1がi2を含まないかチェック
//
// Var("a"), Int(1) ---> true
// Int(1), Var("a") ---> false
// Int(1), Int(2)   ---> false
// Int(1), Int(1)   ---> false
//
function overlapped_item(i1, i2){
    if(i1.tag == "VAR"){
        return true;
    }

    if(i1.tag != i2.tag){
        return false;
    }

    switch(i1.tag){
        case "INT":
        case "BOOL":
        case "STRING":
        {
            return i1.val == i2.val;
        }
        case "TUPLE":
        {
            var ret = true;
            i1.vals.map(function(e, i){
                    ret &= overlapped_item(e, i2.vals[i]);
            });
            return ret;
        }
        case "VARIANT":
        {
            var ret = i1.cons == i2.cons;
            i1.vals.map(function(e, i){
                    ret &= overlapped_item(e, i2.vals[i]);
            });
            return ret;
        }
    }

    return false;
}

function let_expr(l, r, e)
{
    return { type: "LET", l: l, r: r, e: e };
}

function if_expr(l, r, t, f){
    return { type: "IF", l: l, r: r, t: t, f: f };
}

function typeof_expr(target, constructor, t, f){
    return { type: "TYPEOF", target: target, constructor: constructor, t: t, f: f };
}

function ans_expr(e){
    return { type: "ANS", e: e};
}

function err_expr(){
    return { type: "ERR" };
}

function case_expr(target, conds){
    return { type: "CASE", target: target, conds: conds };
}

function cont_expr(e){
    return { type: "CONT", e: e };
}

function get_indent(num){
    var indent = "";
    for(var i = 0; i < num; i++){
        indent += "\t";
    }
    return indent;
}

function expr2string(e, indent){
    var str = "";

    switch(e.type){
        case "LET":{
            str += "LET(" + show_item(e.l) + " = " + show_item(e.r) + " in\n"
                + get_indent(indent + 1) + expr2string(e.e, indent + 1) + ")";
        }
        break;
        case "IF":{
            str += "IF(" + show_item(e.l) + " = " + show_item(e.r)
            + "\n" + get_indent(indent + 1) + " THEN " + expr2string(e.t, indent + 1)
            + "\n" + get_indent(indent + 1) + " ELSE " + expr2string(e.f, indent + 1) + ")";
        }
        break;
        case "TYPEOF":{
            str += "IF (" + show_item(e.target) + " TYPEOF " + e.constructor
            + "\n" + get_indent(indent + 1) + " THEN " + expr2string(e.t, indent + 1)
            + "\n" + get_indent(indent + 1) + " ELSE " + expr2string(e.f, indent + 1) + ")";
        }
        break;
        case "ANS":{
            str += "ANS(" + e.e + ")";
        }
        break;
        case "CASE":{
            str += "CASE(" + show_item(e.target) + ",\n" + e.conds.map(function(e){
                    return get_indent(indent + 1) + show_item(e.cond) + " -> " + expr2string(e.ret, indent + 1);
            }).join("\n") + ")";
        }
        break;
        case "CONT":{
            str += expr2string(e.e, indent);
        }
        break;
        case "ERR":{
            str += "ERROR!";
        }
        break;
    }

    return str;
}

function show_expr(e){
    return expr2string(e, 0);
}

function eq(i1, i2){
    if(i1.tag != i2.tag){
        return false;
    }

    switch(i1.tag){
        case "INT":
        return i1.val == i2.val;
        case "VAR":
        return true;
        case "TUPLE":
        return true;
    }
}

function sep(conds){
    var top_cond = conds[0].cond;
    var ary1 = [conds[0]], ary2 = [];
    for(var i = 1; i < conds.length; i++){
        if(eq(top_cond, conds[i].cond)){
            ary1.push(conds[i]);
        }
        else{
            ary2 = conds.slice(i);
            break;
        }
    }
    return [ary1, ary2];
}

function sep_i(conds){
    var top_cond = conds[0].cond;
    for(var i = 1; i < conds.length; i++){
        if( ! eq(top_cond, conds[i].cond)){
            return i;
        }
    }
    return i;
}

var id_num = 0;
function get_id(){
    return "L" + (id_num++);
}

function expand_item(src){
    switch(src.tag){
        case "TUPLE":
        {
            var vals = src.vals;
            var new_vals = [];
            vals.forEach(function(v){
                    new_vals.push(variable(get_id()));
            });
            return tuple(new_vals);
        }
        case "VARIANT":
        {
            var vals = src.vals;
            var new_vals = [];
            vals.forEach(function(v){
                    new_vals.push(variable(get_id()));
            });
            return variant(src.cons, new_vals);
        }
    }
}

function overlapped_pattern_check(base_cond, remain_conds){
    var overlapped = remain_conds.filter(function(c){
            if(overlapped_item(base_cond.cond, c.cond)){
                return true;
            }
            return false;
    });

    return overlapped.length > 0;
}

function transform(e, cont){
    switch(e.type){
        case "CASE":
        {
            var target = e.target;
            var conds = e.conds;

            // Check overlapped pattern.
            for(var i = 0; i < conds.length - 1; i++){
                var x = conds[i];
                var xs = conds.slice(i + 1);
                if(overlapped_pattern_check(x, xs)){
                    puts("Error: pattern overlapped! " + show_item(x.cond));
                    process.exit(1);
                }
            }

            if(conds[0].cond.tag == "TUPLE"){
                if(target.tag == "TUPLE"){
                    var shifted_target = target.vals[0];
                    var succeeding_target = tuple(target.vals.slice(1));

                    var grouped_conds = [];
                    var remain_conds = conds;

                    var tmp_conds;
                    var base_item;
                    var break_flg;

                    var new_conds = [];

                    if(succeeding_target.vals.length > 0){
                        while(remain_conds.length > 0){
                            tmp_conds = [];
                            base_item = remain_conds[0].cond.vals[0];
                            break_flg = false;

                            //
                            // 分岐対象をグループ化
                            // case (A,B)
                            //  (1,2) -> ..
                            //  (1,3) -> ..
                            //  (2,4) -> ..
                            //
                            //  --->
                            //  (1,2) -> ..
                            //  (1,3) -> ..
                            //  --->
                            //  (2,4) -> ..
                            //
                            remain_conds.forEach(function(c){
                                    if(break_flg){
                                        tmp_conds.push(c);
                                    }
                                    else{
                                        if(equal_item(base_item, c.cond.vals[0])){
                                            grouped_conds.push(c);
                                        }
                                        else{
                                            break_flg = true;
                                            tmp_conds.push(c);
                                        }
                                    }
                            });

                            //
                            // case (A,B)
                            //  (1,2) -> ..
                            //  (1,3) -> ..
                            //  (2,4) -> ..
                            //
                            // --->
                            // case A
                            //  1 -> case (B)
                            //          (2) -> ..
                            //          (3) -> ..
                            //  2 -> case (B)
                            //          (4) -> ..
                            //
                            //
                            var shifted_item = grouped_conds[0].cond.vals[0];

                            var succeeding_conds = [];
                            grouped_conds.forEach(function(c){
                                    succeeding_conds.push({
                                            cond: tuple(c.cond.vals.slice(1)),
                                            ret: c.ret
                                    });
                            });

                            if(shifted_item.tag != "VAR"){

                                //
                                // 以降の条件のうちVARの条件を探す
                                // 存在する場合は失敗時の継続として渡す
                                //
                                // ex)
                                // case (A,B)
                                //  (1,2) -> ...
                                //  (2,3) -> ...
                                //  (a,4) -> ...    <-- (1,2), (2,3)の失敗時にこの条件式へ継続させる
                                //

                                var var_conds = tmp_conds.filter(function(c){
                                        var shifted = c.cond.vals[0];
                                        return shifted.tag == "VAR";
                                });

                                if(var_conds.length > 0){
                                    var new_cont = transform(case_expr(target, var_conds), cont);
                                    new_conds.push({
                                            cond: shifted_item,
                                            ret: transform(case_expr(succeeding_target, succeeding_conds), new_cont)
                                    });
                                }
                                else{
                                    new_conds.push({
                                            cond: shifted_item,
                                            ret: transform(case_expr(succeeding_target, succeeding_conds), cont)
                                    });
                                }

                            }
                            else{
                                if(tmp_conds.length > 0){
                                    // 以降の条件は並列に扱えないため全て継続として渡す
                                    var new_cont = transform(case_expr(target, tmp_conds), cont);
                                    new_conds.push({
                                            cond: shifted_item,
                                            ret: transform(case_expr(succeeding_target, succeeding_conds), new_cont)
                                    });
                                    tmp_conds = [];
                                }
                                else{
                                    new_conds.push({
                                            cond: shifted_item,
                                            ret: transform(case_expr(succeeding_target, succeeding_conds), cont)
                                    });
                                }
                            }

                            // 初期化
                            grouped_conds = [];
                            remain_conds = tmp_conds;
                        }
                    }
                    else{
                        conds.forEach(function(c){
                                new_conds.push({
                                        cond: c.cond.vals[0],
                                        ret: c.ret
                                });
                        });
                    }

                    var new_case = case_expr(shifted_target, new_conds);
                    return transform(new_case, cont);
                }
                else{
                    var tuple_conds = [];
                    var remain_conds = [];
                    var break_flg = false;
                    conds.forEach(function(c){
                            if(break_flg){
                                remain_conds.push(c);
                            }
                            else{
                                if(c.cond.tag == "TUPLE"){
                                    tuple_conds.push(c);
                                }
                                else{
                                    break_flg = true;
                                    remain_conds.push(c);
                                }
                            }
                    });

                    var new_target = expand_item(conds[0].cond);
                    var new_cont = cont_expr(null);
                    var new_case = case_expr(new_target, tuple_conds);
                    var new_expr = transform(new_case, new_cont);
                    if(remain_conds.length > 0){
                        var remain_case = case_expr(target, remain_conds);
                        new_cont.e = transform(remain_case, cont);
                    }
                    else{
                        new_cont.e = cont;
                    }
                    return let_expr(new_target, target, new_expr);
                }
            }
            else if(conds[0].cond.tag == "VARIANT"){

                var constructor = conds[0].cond.cons;

                var grouped_conds = [];
                var remain_conds = [];

                var base_item = conds[0].cond;
                var break_flg = false;

                conds.forEach(function(c){
                        if(break_flg){
                            remain_conds.push(c);
                        }
                        else{
                            if(c.cond.tag == "VARIANT"){
                                if(base_item.cons == c.cond.cons){
                                    grouped_conds.push(c);
                                }
                                else{
                                    break_flg = true;
                                    remain_conds.push(c);
                                }
                            }
                            else{
                                remain_conds.push(c);
                            }
                        }
                });

                //
                // TupleのCase式へ変換する
                //
                // case T
                //  (Cons 1 xs) -> ..
                //  (Cons 2 xs) -> ..
                //
                // --->
                // let (Cons a b) = T in
                // case (a, b)
                //  (1, xs) -> ..
                //  (2, xs) -> ..
                //

                var new_conds = grouped_conds.map(function(c){
                        return {
                            cond: tuple(c.cond.vals),
                            ret: c.ret
                        };
                });

                // 後続の条件式を変換
                var new_cont = cont_expr(null);
                if(remain_conds.length > 0){
                    var remain_case = case_expr(target, remain_conds);
                    new_cont.e = transform(remain_case, cont);
                }
                else{
                    new_cont.e = cont;
                }

                // コンストラクタが必要とする引数の数により束縛するか決定する
                if(base_item.vals.length > 0){
                    var bind_item = expand_item(base_item);
                    var new_case = transform(case_expr(tuple(bind_item.vals), new_conds), new_cont);

                    // ex) Cons x xs
                    return typeof_expr(target, constructor,
                        let_expr(bind_item, target, new_case), new_cont);
                }
                else{
                    // ex) Nil
                    var new_expr = transform(grouped_conds[0].ret, cont);   // cont? new_cont?
                    return typeof_expr(target, constructor, new_expr, new_cont);
                }
            }
            else if(conds[0].cond.tag == "VAR"){
                if(conds.length > 1){
                    puts("Error: pattern overlapped! " + show_item(conds[0].cond));
                    process.exit(1);
                }

                //
                // 不要なCase式を除去
                //
                // case T
                //  x -> ..
                //
                // --->
                // let x = T in ...
                //
                var c = e.conds[0];
                return let_expr(c.cond, target, transform(c.ret, cont));
            }
            else{
                var new_conds = [];
                var has_variable = false;
                conds.forEach(function(c){
                        if(c.cond.tag == "VAR"){
                            has_variable = true;
                        }
                        new_conds.push({
                                cond: c.cond,
                                ret: transform(c.ret, cont)
                        });
                });
                if( ! has_variable){
                    new_conds.push({
                            cond: variable("_"),
                            ret: cont
                    });
                }

                return case_expr(target, new_conds);
            }
        }
    }
    return e;
}

var exp0 = case_expr(variable("X"), [
        { cond: variable("a"), ret: ans_expr("A") }
]);

var exp1 = case_expr(variable("X"), [
        { cond: integer(10), ret: ans_expr("A") },
        { cond: integer(20), ret: ans_expr("B") },
        { cond: variable("a"), ret: ans_expr("C") }
]);

var exp2 = case_expr(variable("X"), [
        { cond: tuple([integer(10), integer(20)]), ret: ans_expr("A") },
        { cond: tuple([integer(20), integer(30)]), ret: ans_expr("B") },
        { cond: variable("a"), ret: ans_expr("C") }
]);

var exp3 = case_expr(variable("X"), [
        { cond: tuple([integer(10), integer(20)]), ret: ans_expr("A") },
        { cond: tuple([integer(10), integer(30)]), ret: ans_expr("B") },
        { cond: tuple([integer(20), integer(30)]), ret: ans_expr("C") },
        { cond: variable("_"), ret: ans_expr("TRASH") }
]);

var exp4 = case_expr(variable("X"), [
        { cond: tuple([integer(10), tuple([integer(10), integer(20)])]), ret: ans_expr("A") },
        { cond: tuple([integer(10), tuple([integer(20), integer(20)])]), ret: ans_expr("B") },
        { cond: tuple([integer(20), variable("x")]), ret: ans_expr("C") },
        { cond: variable("_"), ret: ans_expr("TRASH") }
]);

var exp5 = case_expr(variable("X"), [
        { cond: variant("Nil", []), ret: ans_expr("A") },
        { cond: variant("Cons", [integer(1), variable("xs")]), ret: ans_expr("B") },
        { cond: variant("Cons", [integer(2), variant("Cons", [integer("1"), variable("xs")])]), ret: ans_expr("C-1") },
        { cond: variant("Cons", [integer(2), variant("Cons", [variable("y"), variable("xs")])]), ret: ans_expr("C-2") },
        { cond: variable("_"), ret: ans_expr("TRASH") }
]);

var exp6 = case_expr(variable("X"), [
        { cond: tuple([integer(10), integer(20)]), ret: ans_expr("A") },
        { cond: tuple([integer(10), integer(30)]), ret: ans_expr("B") },
        { cond: tuple([variable("a"), integer(30)]), ret: ans_expr("C") },
        { cond: tuple([variable("a"), integer(40)]), ret: ans_expr("D") },
        { cond: tuple([integer(30), integer(50)]), ret: ans_expr("E") },
]);

var exp7 = case_expr(variable("X"), [
        { cond: tuple([tuple([integer(10), integer(20)]), tuple([integer(10), integer(20)])]), ret: ans_expr("A") },
        { cond: tuple([tuple([integer(10), integer(20)]), tuple([integer(20), integer(20)])]), ret: ans_expr("B") },
        { cond: variable("_"), ret: ans_expr("TRASH") }
]);

puts(show_expr(transform(exp0, err_expr())));
puts(show_expr(transform(exp1, err_expr())));
puts(show_expr(transform(exp2, err_expr())));
puts(show_expr(transform(exp3, err_expr())));
puts(show_expr(transform(exp4, err_expr())));
puts(show_expr(transform(exp5, err_expr())));
puts(show_expr(transform(exp6, err_expr())));
puts(show_expr(transform(exp7, err_expr())));

