#include <stdlib.h>
#include <vector>
#include <string>
#include <iostream>
#include <stack>
#include <cmath>

typedef std::vector <std::pair <int, std::string>> tokens_t;  // type for tokens (type - value)
typedef union Num{
    int inum;
    double fnum;
} num_t;

enum token_types{
    I_NUM,
    F_NUM,
    ADD,
    SUB,
    MUL,
    DIV,
    MOD,
    POW,
    O_PRN,
    C_PRN,
    ENDOFFILE
};

enum node_types{
    INUM_NODE,
    FNUM_NODE,
    UN_OP,
    BI_OP
};

class Node{
private:
    int node_type;
    std::string num;
    std::string op;

public:
    Node(){
        node_type = 0;
        num = "";
        op = "";
    }

    void set_type(int value){
        node_type = value;
    }
    void set_num(std::string value){
        num = value;
    }
    void set_op(std::string value){
        op = value;
    }

    int check_type(){
        return node_type;
    }
    std::string check_num(){
        return num;
    }
    std::string check_op(){
        return op;
    }
};

class Parser{
private:
    size_t current;
    std::vector <Node> nodes;

    void push_node(Node node){
        nodes.emplace_back(node);
    }

    void parse_initial(const tokens_t &tokens){
        // (EXPR)
        if (tokens[current].first == O_PRN){
            inc_cur();
            parse_addition(tokens);
            inc_cur();
            if (tokens[current].first != C_PRN){
                std::cout << "Need push C_PRN node!!!";     // ???
            }
            return;
        }

        // UNOP
        if (tokens[current].first == SUB || tokens[current].first == ADD ||
            tokens[current].first == DIV || tokens[current].first == MUL ||
            tokens[current].first == MOD || tokens[current].first == POW){
            Node node;
            node.set_type(UN_OP);
            node.set_op(tokens[current].second);

            inc_cur();
            parse_initial(tokens);

            push_node(node);
            return;
        }

        // I_NUM
        if (tokens[current].first == I_NUM){
            Node node;
            node.set_type(INUM_NODE);
            node.set_num(tokens[current].second);
            push_node(node);
            return;
        }

        // F_NUM
        if (tokens[current].first == F_NUM){
            Node node;
            node.set_type(FNUM_NODE);
            node.set_num(tokens[current].second);
            push_node(node);
            return;
        }
    }

    void parse_pow(const tokens_t &tokens){
        parse_initial(tokens);
        inc_cur();
        while (tokens[current].first == POW){
            Node node;
            node.set_type(BI_OP);
            node.set_op(tokens[current].second);
            inc_cur();
            parse_initial(tokens);
            push_node(node);
            inc_cur();
        }
        dec_cur();
    }

    void parse_multiplication(const tokens_t& tokens){
        parse_pow(tokens);
        inc_cur();
        while (tokens[current].first == MUL || tokens[current].first == DIV || tokens[current].first == MOD){
            Node node;
            node.set_type(BI_OP);
            node.set_op(tokens[current].second);
            inc_cur();
            parse_pow(tokens);
            push_node(node);
            inc_cur();
        }
        dec_cur();
    }

public:
    Parser(){
        current = 0;
        nodes = {};
    }

    void inc_cur(){
        current++;
    }
    void dec_cur(){
        current--;
    }

    size_t out_cur(){
        return current;
    }
    std::vector<Node> nodes_out(){
        return nodes;
    }

    void parse_addition(const tokens_t &tokens){
        parse_multiplication(tokens);
        inc_cur();
        while (tokens[current].first == ADD || tokens[current].first == SUB){
            Node node;
            node.set_type(BI_OP);
            node.set_op(tokens[current].second);
            inc_cur();
            parse_multiplication(tokens);
            push_node(node);
            inc_cur();
        }
        dec_cur();
    }
};

tokens_t tokenizer(const std::string& input){
    tokens_t tokens;    // list of tokens.
    size_t current = 0;
    char symbol = input[current];
    while (current < input.size()){
        std::string value;

        // EOF
        if (symbol == 'q'){
            tokens.emplace_back(ENDOFFILE, "EOF");
            break;
        }
        // Skip spaces
        if (symbol == ' '){
            symbol = input[++current];
            continue;
        }

        // Numbers
        if (std::isdigit(symbol)){
            value = "";
            value += symbol;
            symbol = input[++current];
            int type = I_NUM;
            while (std::isdigit(symbol) || symbol == '.'){
                if (symbol == '.'){
                    type = F_NUM;
                }
                value += symbol;
                symbol = input[++current];
            }
            tokens.emplace_back(type, value);
            continue;
        }

        // Addition
        if (symbol == '+'){
            value = "";
            value += symbol;
            symbol = input[++current];
            tokens.emplace_back(ADD, value);
            continue;
        }

        // Subtraction
        if (symbol == '-'){
            value = "";
            value += symbol;
            symbol = input[++current];
            tokens.emplace_back(SUB, value);
            continue;
        }

        // Multiplication
        if (symbol == '*'){
            value = "";
            value += symbol;
            symbol = input[++current];
            tokens.emplace_back(MUL, value);
            continue;
        }

        // Division
        if (symbol == '/'){
            value = "";
            value += symbol;
            symbol = input[++current];
            tokens.emplace_back(DIV, value);
            continue;
        }

        // Mod
        if (symbol == '%'){
            value = "";
            value += symbol;
            symbol = input[++current];
            tokens.emplace_back(MOD, value);
            continue;
        }

        // Power
        if (symbol == '^'){
            value = "";
            value += symbol;
            symbol = input[++current];
            tokens.emplace_back(POW, value);
            continue;
        }

        // Open parentheses
        if (symbol == '('){
            value = "";
            value += symbol;
            symbol = input[++current];
            tokens.emplace_back(O_PRN, value);
            continue;
        }

        // Close parentheses
        if (symbol == ')'){
            value = "";
            value += symbol;
            symbol = input[++current];
            tokens.emplace_back(C_PRN, value);
            continue;
        }

        std::cout << "Wrong input " << symbol << std::endl;
        exit(0);
    }
    return tokens;
}

bool double_int(const std::string& result){
    bool success = false;
    for (size_t i = 0; i < result.size(); i++){
        if (result[i] == '.') {
            success = true;
            break;
        }
    }
    return success;
}

std::string result(std::vector<Node>& nodes){
    std::stack<std::string> stack;
    size_t current = 0;

    double numr, numl;
    double res;
    std::string ret;
    num_t num;

    while (current < nodes.size()){
        switch (nodes[current].check_type()){
            case INUM_NODE:
                stack.push(nodes[current].check_num());
                current++;
                break;

            case FNUM_NODE:
                stack.push(nodes[current].check_num());
                current++;
                break;

            case BI_OP:
                numr = std::stod(stack.top());
                stack.pop();
                numl = std::stod(stack.top());
                stack.pop();
                if (nodes[current].check_op() == "^"){
                    res = pow(numl, numr);
                    ret = "";
                    ret += std::to_string(res);
                    stack.push(ret);
                    current++;
                    break;
                }
                if (nodes[current].check_op() == "%"){
                    res = (int)numl % (int)numr;
                    ret = "";
                    ret += std::to_string(res);
                    stack.push(ret);
                    current++;
                    break;
                }
                if (nodes[current].check_op() == "+"){
                    res = numl + numr;
                    ret = "";
                    ret += std::to_string(res);
                    stack.push(ret);
                    current++;
                    break;
                }
                if (nodes[current].check_op() == "-"){
                    res = numl - numr;
                    ret = "";
                    ret += std::to_string(res);
                    stack.push(ret);
                    current++;
                    break;
                }
                if (nodes[current].check_op() == "*"){
                    res = numl * numr;
                    ret = "";
                    ret += std::to_string(res);
                    stack.push(ret);
                    current++;
                    break;
                }
                if (nodes[current].check_op() == "/"){
                    res = numl / numr;
                    ret = "";
                    ret += std::to_string(res);
                    stack.push(ret);
                    current++;
                    break;
                }

            case UN_OP:
                numr = std::stod(stack.top());
                stack.pop();
                if (nodes[current].check_op() == "^"){
                    res = pow(0, numr);
                    ret = "";
                    ret += std::to_string(res);
                    stack.push(ret);
                    current++;
                    break;
                }
                if (nodes[current].check_op() == "%"){
                    res = 0 % (int)numr;
                    ret = "";
                    ret += std::to_string(res);
                    stack.push(ret);
                    current++;
                    break;
                }
                if (nodes[current].check_op() == "+"){
                    res = 0 + numr;
                    ret = "";
                    ret += std::to_string(res);
                    stack.push(ret);
                    current++;
                    break;
                }
                if (nodes[current].check_op() == "-"){
                    res = 0 - numr;
                    ret = "";
                    ret += std::to_string(res);
                    stack.push(ret);
                    current++;
                    break;
                }
                if (nodes[current].check_op() == "*"){
                    res = 0 * numr;
                    ret = "";
                    ret += std::to_string(res);
                    stack.push(ret);
                    current++;
                    break;
                }
                if (nodes[current].check_op() == "/"){
                    res = 0 / numr;
                    ret = "";
                    ret += std::to_string(res);
                    stack.push(ret);
                    current++;
                    break;
                }
            default:
                std::cout << "ERROR" << std::endl;
        }
    }
    return ret;
}

void tokens_out(tokens_t tokens){
    for (size_t i = 0; i < tokens.size(); i++){
        std::cout << tokens[i].first << " -> " << tokens[i].second << std::endl;
    }
}

int main(int argc, char** argv){
    std::string input = "2.9%5.6";
    input += 'q';
    tokens_t tokens = tokenizer(input);
//    tokens_out(tokens);

    Parser p;
    while (p.out_cur() < tokens.size()-1){
        p.parse_addition(tokens);
        p.inc_cur();
    }
    std::vector<Node> nodes = p.nodes_out();

    std::cout << result(nodes) << std::endl;
    return 0;
}