type Variable = [Char]
type Constructor = [Char]
type Ident = [Char]
data Pattern = Var Variable | Con Constructor [Pattern] deriving (Show)

arity :: Constructor -> Int
arity c = case c of
    "NIL" -> 0
    "CONS" -> 2

constructors :: Constructor -> [Constructor]
constructors c = case c of
    "NIL" -> ["NIL", "CONS"]
    "CONS" -> ["NIL", "CONS"]

data Clause = Clause Constructor [Variable] Expression deriving (Show)
data Expression = Case Variable [Clause] | Fatbar Expression Expression | App Ident [Variable] deriving (Show)

subst :: Expression -> Variable -> Variable -> Expression
subst (Case v cls) to from =
    let v' = substVar to from v in
    let cls' = map (substClause to from) cls in
    Case v' cls'
subst (Fatbar e1 e2) to from =
    let e1' = subst e1 to from in
    let e2' = subst e2 to from in
    Fatbar e1' e2'
subst (App id vs) to from =
    let vs' = map (substVar to from) vs in
    App id vs'

substVar to from v = if v == from then to else v
substClause to from (Clause c vs e) =
    let vs' = map (substVar to from) vs in
    let e' = subst e to from in
    Clause c vs' e'


type Equation = ([Pattern], Expression)

isVar :: Equation -> Bool
isVar (((Var v):ps), e) = True
isVar (((Con c ps'):ps), e) = False

isCon :: Equation -> Bool
isCon q = not (isVar q)

getCon :: Equation -> Constructor
getCon (((Con c ps'):ps), e) = c

makeVar :: Int -> Variable
makeVar k = "_u" ++ show k

partition f [] = []
partition f [x] = [[x]]
partition f (x:x':xs) =
    if f x == f x'
        then tack x (partition f (x':xs))
        else [x] : partition f (x': xs)
tack x xss = (x : head xss) : tail xss

match :: Int -> [Variable] -> [Equation] -> Expression -> Expression
match k [] qs def = foldr Fatbar def [e|([],e) <- qs]
match k (u:us) qs def = foldr (matchVarCon k (u:us)) def (partition isVar qs)

matchVarCon k us qs def
    | isVar (head qs) = matchVar k us qs def
    | isCon (head qs) = matchCon k us qs def

matchVar k (u:us) qs def = match k us [(ps, subst e u v) | (Var v : ps, e) <- qs] def
matchCon k (u:us) qs def = Case u [matchClause c k (u:us) (choose c qs) def | c <- cs]
    where cs = constructors (getCon (head qs))

matchClause c k (u:us) qs def =
    Clause c us' (match (k'+k)
                        (us'++us)
                        [(ps'++ps, e) | (Con c ps' : ps, e) <- qs]
                        def)
    where
        k' = arity c
        us' = [makeVar (i+k) | i <- [1..k']]

choose c qs = [q | q <- qs, getCon q == c]

e0 = App "f" []
e1 = App "f" ["ys"]
e2 = App "f" ["x", "xs"]
e3 = App "f" ["x", "y"]
test = match 3
             ["_u1", "_u2", "_u3"]
             [ ([Var "f", Con "NIL" [], Var "ys"], e1),
               ([Var "f", Con "CONS" [Var "x", Var "xs"], Con "NIL" []], e2),
               ([Var "f", Con "CONS" [Var "x", Var "xs"], Con "CONS" [Var "y", Var "ys"]], e3) ]
             e0
{-
match (u1, u2, u3)
    (f, [], ys) -> f ys
    (f, (x:xs) []) -> f x xs
    (f, (x:xs) (y:ys)) -> f x y
-}

{-
Case "_u2"
    [Clause "NIL" [] (Fatbar (App "f" ["_u3"]) (App "f" [])),
     Clause "CONS" ["_u4","_u5"] (
        Case "_u3" [
            Clause "NIL" [] (Fatbar (App "f" ["_u4","_u5"]) (App "f" [])),
            Clause "CONS" ["_u6","_u7"] (Fatbar (App "f" ["_u4","_u6"]) (App "f" []))])]

case u2 of
    [] -> f u3
    (u4:u5) ->
        case u3 of
            [] -> f u4 u5
            (u6:u7) -> f u4 u6

-}

main = print test
