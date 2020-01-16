#include <stdlib.h>
#include <vector>
#include <string>
#include <iostream>
#include <stack>

typedef std::vector <std::pair <int, std::string>> tokens_t;  // type for tokens (type - value)

enum token_types{
    NUM,
    ADD,
    SUB,
    MUL,
    DIV,
    O_PRN,
    C_PRN,
    ENDOFFILE
};

enum node_types{
    N_NUM,
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
            tokens[current].first == DIV || tokens[current].first == MUL){
            Node node;
            node.set_type(UN_OP);
            node.set_op(tokens[current].second);

            inc_cur();
            parse_initial(tokens);

            push_node(node);
            return;
        }

        // NUM
        if (tokens[current].first == NUM){
            Node node;
            node.set_type(N_NUM);
            node.set_num(tokens[current].second);
            push_node(node);
            return;
        }
    }

    void parse_multiplication(const tokens_t &tokens){
        parse_initial(tokens);
        inc_cur();
        while (tokens[current].first == MUL || tokens[current].first == DIV){
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

        // Integers
        if (std::isdigit(symbol)){
            value = "";
            value += symbol;
            symbol = input[++current];
            while (std::isdigit(symbol)){
                value += symbol;
                symbol = input[++current];
            }
            tokens.emplace_back(NUM, value);
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

int result(std::vector<Node>& nodes){
    std::stack<std::string> stack;
    size_t current = 0;

    int numr, numl;
    std::string res;
    while (current < nodes.size()){
        switch (nodes[current].check_type()){
            case N_NUM:
                stack.push(nodes[current].check_num());
                current++;
                break;

            case BI_OP:
                numr = std::stoi(stack.top());
                stack.pop();
                numl = std::stoi(stack.top());
                stack.pop();
                if (nodes[current].check_op() == "+"){
                    res = "";
                    res += std::to_string(numl + numr);
                    stack.push(res);
                    current++;
                    break;
                }
                if (nodes[current].check_op() == "-"){
                    res = "";
                    res += std::to_string(numl - numr);
                    stack.push(res);
                    current++;
                    break;
                }
                if (nodes[current].check_op() == "*"){
                    res = "";
                    res += std::to_string(numl * numr);
                    stack.push(res);
                    current++;
                    break;
                }
                if (nodes[current].check_op() == "/"){
                    res = "";
                    res += std::to_string(numl / numr);
                    stack.push(res);
                    current++;
                    break;
                }

            case UN_OP:
                numr = std::stoi(stack.top());
                stack.pop();
                if (nodes[current].check_op() == "-"){
                    res = "";
                    res += std::to_string(0 - numr);
                    stack.push(res);
                    current++;
                    break;
                }
                if (nodes[current].check_op() == "+"){
                    res = "";
                    res += std::to_string(0 + numr);
                    stack.push(res);
                    current++;
                    break;
                }
                if (nodes[current].check_op() == "*"){
                    res = "";
                    res += std::to_string(0 * numr);
                    stack.push(res);
                    current++;
                    break;
                }
                if (nodes[current].check_op() == "/"){
                    res = "";
                    res += std::to_string(0 / numr);
                    stack.push(res);
                    current++;
                    break;
                }

            default:
                std::cout << "ERROR" << std::endl;
        }
    }
    return std::stoi(stack.top());
}

void tokens_out(tokens_t tokens){
    for (size_t i = 0; i < tokens.size(); i++){
        std::cout << tokens[i].first << " -> " << tokens[i].second << std::endl;
    }
}

int main(int argc, char** argv){
    std::string input = "-9";
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