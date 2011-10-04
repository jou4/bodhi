var regs = [null, null, null];
var stack = [];

var vals = {};

var expr = [
    ["a", 10],
    ["b", "a", "a"],
    ["c", "b", "b"],
    ["d", "c", "c"],
    ["e", "d", "d"],
    ["f", "e", "e"],
    ["g", "d", "e", "f"]
];

function live_range(name, begin, expr)
{
    var end = begin;
    var e;
    for(var i = begin + 1; i < expr.length; ++i){
        e = expr[i];
        if(e.indexOf(name) >= 0){
            end = i;
        }
    }
    return [name, begin, end, "", -1];
}

function clean_reg(curr)
{
    for(var i = 0; i < regs.length; i++){
        if(regs[i] && regs[i][2] < curr){
            regs[i][3] = "remove";
            rm_reg(i);
        }
    }
}

function disused_reg()
{
    var index = -1;
    for(var i = 0; i < regs.length; i++){
        if( ! regs[i]){
            index = i;
            break;
        }
    }
    return index;
}

function latest()
{
    var index = -1;
    var end = -1;
    for(var i = 0; i < regs.length; i++){
        if(regs[i][2] > end){
            index = i;
            end = regs[i][2];
        }
    }
    return index;
}

function set_reg(reg_i, val)
{
    regs[reg_i] = val;
}

function rm_reg(reg_i)
{
    regs[reg_i] = null;
}

function store(val)
{
    stack.push(val);
    return stack.length - 1;
}

function load(reg_i, stack_i)
{
    regs[reg_i] = stack[stack_i];
}


function regalloc()
{
    var e;
    var name;
    var live;
    var reg_i;
    var stack_i;
    var latest_i;
    for(var i = 0; i < expr.length; ++i){
        e = expr[i];
        name = e[0];
        live = live_range(name, i, expr);
        vals[name] = live;

        clean_reg(i);
        reg_i = disused_reg();

        if(reg_i < 0){
            latest_i = latest();
            var spill_val = regs[latest_i];

            if(spill_val[2] > live[2]){
                console.log("*spilled*" + spill_val[0]);

                stack_i = store(spill_val);
                spill_val[3] = "stack";
                spill_val[4] = stack_i;

                reg_i = latest_i;
            }
            else{
                // spill itself
                spill_val = live;

                console.log("*spilled itself*" + spill_val[0]);

                stack_i = store(spill_val);
                spill_val[3] = "stack";
                spill_val[4] = stack_i;

                continue;
            }

        }

        live[3] = "reg";
        live[4] = reg_i;
        set_reg(reg_i, live);

        console.log(regs);
    }
    console.log("");
    console.log("registers");
    console.log(regs);

    console.log("");
    console.log("stack");
    console.log(stack);

    console.log("");
    console.log("values");
    console.log(vals);
}

regalloc();
