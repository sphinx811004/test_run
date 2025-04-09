#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <stdexcept>
#include <fstream>

// Global variables map
std::map<std::string, int> variables;

// Token types
enum class TokenType {
    NUMBER,
    PLUS,
    MINUS,
    MULTIPLY,
    DIVIDE,
    ASSIGN,
    IDENTIFIER,
    SQUARE,
    END
};

// Token structure
struct Token {
    TokenType type;
    std::string value;
    
    Token(TokenType t, std::string v = "") : type(t), value(v) {}
};

// AST Node base class
class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual int evaluate() = 0;
    virtual std::string toAssembly() = 0;
};

// Number node
class NumberNode : public ASTNode {
    int value;
public:
    NumberNode(int v) : value(v) {}
    int evaluate() override { return value; }
    std::string toAssembly() override {
        return "    mov eax, " + std::to_string(value) + "\n";
    }
    int getValue() const { return value; }
};

// Binary operation node
class BinaryOpNode : public ASTNode {
    char op;
    std::unique_ptr<ASTNode> left, right;
public:
    BinaryOpNode(char o, std::unique_ptr<ASTNode> l, std::unique_ptr<ASTNode> r)
        : op(o), left(std::move(l)), right(std::move(r)) {}
    
    int evaluate() override {
        int l = left->evaluate();
        int r = right->evaluate();
        switch(op) {
            case '+': return l + r;
            case '-': return l - r;
            case '*': return l * r;
            case '/': return l / r;
            default: throw std::runtime_error("Unknown operator");
        }
    }
    
    std::string toAssembly() override {
        std::string code = right->toAssembly();  // Evaluate right operand
        code += "    push eax\n";                // Save right result
        code += left->toAssembly();              // Evaluate left operand
        code += "    pop ebx\n";                 // Restore right result
        
        switch(op) {
            case '+': return code + "    add eax, ebx\n";
            case '-': return code + "    sub eax, ebx\n";
            case '*': return code + "    imul eax, ebx\n";
            case '/': return code + "    cdq\n    idiv ebx\n";
            default: throw std::runtime_error("Unknown operator");
        }
    }
};

// Square operation node
class SquareNode : public ASTNode {
    std::unique_ptr<ASTNode> operand;
public:
    SquareNode(std::unique_ptr<ASTNode> op) : operand(std::move(op)) {}
    
    int evaluate() override {
        int val = operand->evaluate();
        return val * val;
    }
    
    std::string toAssembly() override {
        std::string code = operand->toAssembly();
        code += "    square eax\n";  // Use the square macro
        return code;
    }
    
    // Make operand accessible for debugging
    const std::unique_ptr<ASTNode>& getOperand() const { return operand; }
};

// Assignment node
class AssignmentNode : public ASTNode {
    std::string name;
    std::unique_ptr<ASTNode> value;
public:
    AssignmentNode(std::string n, std::unique_ptr<ASTNode> v)
        : name(n), value(std::move(v)) {}
    
    int evaluate() override {
        int val = value->evaluate();
        variables[name] = val;
        return val;
    }
    
    std::string toAssembly() override {
        std::string code = value->toAssembly();
        code += "    mov [" + name + "], eax\n";
        return code;
    }
    
    const std::unique_ptr<ASTNode>& getValue() const { return value; }
};

// Variable node
class VariableNode : public ASTNode {
    std::string name;
public:
    VariableNode(std::string n) : name(n) {}
    
    int evaluate() override {
        auto it = variables.find(name);
        if (it == variables.end()) throw std::runtime_error("Undefined variable: " + name);
        return it->second;
    }
    
    std::string toAssembly() override {
        return "    mov eax, [" + name + "]\n";
    }
};

// Lexer class
class Lexer {
    std::string input;
    size_t pos = 0;
    
    char peek() {
        if (pos >= input.length()) return '\0';
        return input[pos];
    }
    
    char advance() {
        if (pos >= input.length()) return '\0';
        return input[pos++];
    }
    
    void skipWhitespace() {
        while (peek() == ' ' || peek() == '\t' || peek() == '\n') advance();
    }
    
public:
    Lexer(std::string in) : input(in) {}
    
    Token getNextToken() {
        skipWhitespace();
        
        if (peek() == '\0') return Token(TokenType::END);
        
        if (isdigit(peek())) {
            std::string num;
            while (isdigit(peek())) num += advance();
            std::cout << "Token: NUMBER " << num << std::endl;
            return Token(TokenType::NUMBER, num);
        }
        
        if (isalpha(peek())) {
            std::string id;
            while (isalnum(peek())) id += advance();
            std::cout << "Token: IDENTIFIER " << id << std::endl;
            return Token(TokenType::IDENTIFIER, id);
        }
        
        char c = advance();
        switch(c) {
            case '+': 
                std::cout << "Token: PLUS" << std::endl;
                return Token(TokenType::PLUS);
            case '-': 
                std::cout << "Token: MINUS" << std::endl;
                return Token(TokenType::MINUS);
            case '*': 
                std::cout << "Token: MULTIPLY" << std::endl;
                return Token(TokenType::MULTIPLY);
            case '/': 
                std::cout << "Token: DIVIDE" << std::endl;
                return Token(TokenType::DIVIDE);
            case '=': 
                std::cout << "Token: ASSIGN" << std::endl;
                return Token(TokenType::ASSIGN);
            case '^': 
                std::cout << "Token: SQUARE" << std::endl;
                return Token(TokenType::SQUARE);
            default: 
                std::cout << "Token: INVALID " << c << std::endl;
                throw std::runtime_error("Invalid character");
        }
    }
};

// Parser class
class Parser {
    Lexer lexer;
    Token currentToken{TokenType::END};  // Initialize with END token
    
    void eat(TokenType type) {
        if (currentToken.type == type) {
            currentToken = lexer.getNextToken();
        } else {
            throw std::runtime_error("Unexpected token");
        }
    }
    
public:
    Parser(Lexer l) : lexer(l) {
        currentToken = lexer.getNextToken();
    }
    
    std::unique_ptr<ASTNode> parse() {
        if (currentToken.type == TokenType::IDENTIFIER) {
            std::string name = currentToken.value;
            eat(TokenType::IDENTIFIER);
            
            if (currentToken.type == TokenType::ASSIGN) {
                eat(TokenType::ASSIGN);
                auto value = parseExpr();
                return std::make_unique<AssignmentNode>(name, std::move(value));
            }
            return std::make_unique<VariableNode>(name);
        }
        return parseExpr();
    }
    
private:
    std::unique_ptr<ASTNode> parseExpr() {
        auto node = parseTerm();
        
        while (currentToken.type == TokenType::PLUS || 
               currentToken.type == TokenType::MINUS) {
            char op = currentToken.type == TokenType::PLUS ? '+' : '-';
            eat(currentToken.type);
            node = std::make_unique<BinaryOpNode>(op, std::move(node), parseTerm());
        }
        
        return node;
    }
    
    std::unique_ptr<ASTNode> parseTerm() {
        auto node = parseFactor();
        
        while (currentToken.type == TokenType::MULTIPLY || 
               currentToken.type == TokenType::DIVIDE) {
            char op = currentToken.type == TokenType::MULTIPLY ? '*' : '/';
            eat(currentToken.type);
            node = std::make_unique<BinaryOpNode>(op, std::move(node), parseFactor());
        }
        
        return node;
    }
    
    std::unique_ptr<ASTNode> parseFactor() {
        std::unique_ptr<ASTNode> node;
        
        if (currentToken.type == TokenType::NUMBER) {
            int value = std::stoi(currentToken.value);
            eat(TokenType::NUMBER);
            node = std::make_unique<NumberNode>(value);
        }
        else if (currentToken.type == TokenType::IDENTIFIER) {
            std::string name = currentToken.value;
            eat(TokenType::IDENTIFIER);
            node = std::make_unique<VariableNode>(name);
        }
        else {
            throw std::runtime_error("Unexpected token in factor");
        }
        
        // Check for square operation after the number or identifier
        if (currentToken.type == TokenType::SQUARE) {
            eat(TokenType::SQUARE);
            node = std::make_unique<SquareNode>(std::move(node));
        }
        
        return node;
    }
};

// Assembly generator
class AssemblyGenerator {
    std::vector<std::string> variables;
    
public:
    std::string generateHeader() {
        return "section .data\n";
    }
    
    std::string generateVariable(const std::string& name) {
        variables.push_back(name);
        return name + ": dd 0\n";  // Changed to use colon notation for labels on macOS
    }
    
    std::string generateCode(const std::string& assembly) {
        std::string code = "; Square operation macro\n";
        code += "%macro square 1\n";
        code += "    ; Save operand\n";
        code += "    mov ebx, %1\n";
        code += "    ; Square the value\n";
        code += "    imul %1, ebx\n";
        code += "%endmacro\n\n";
        
        code += "section .text\n";
        code += "global _main\n";
        code += "extern _printf\n\n";  // Removed Windows-specific ExitProcess

        code += "_main:\n";
        code += "    push ebp\n";
        code += "    mov ebp, esp\n";
        code += assembly;
        code += "    push eax\n";        // Save result
        code += "    push dword [x]\n";  // Push value to print
        code += "    push dword fmt\n";  // Push format string
        code += "    call _printf\n";    // Call printf
        code += "    add esp, 12\n";     // Clean up stack (3 args × 4 bytes)
        code += "    pop eax\n";         // Restore result

        // macOS exit routine
        code += "    xor eax, eax\n";    // Return 0 success code
        code += "    mov esp, ebp\n";
        code += "    pop ebp\n";
        code += "    ret\n\n";           // Return from main instead of ExitProcess call
        
        code += "section .data\n";
        code += "fmt: db 'Result: %d', 10, 0\n";  // Changed to use colon notation for labels
        return code;
    }
};

int main() {
    try {
        std::string input = "x = 5 ^ 2";  // Example input: square of 5
        
        // Generate assembly
        Lexer lexer(input);
        Parser parser(lexer);
        auto ast = parser.parse();
        
        // Debug: Print the AST structure
        std::cout << "AST Structure: ";
        if (dynamic_cast<AssignmentNode*>(ast.get())) {
            std::cout << "AssignmentNode" << std::endl;
            auto* assignNode = dynamic_cast<AssignmentNode*>(ast.get());
            if (dynamic_cast<SquareNode*>(assignNode->getValue().get())) {
                std::cout << "  - SquareNode" << std::endl;
                auto* squareNode = dynamic_cast<SquareNode*>(assignNode->getValue().get());
                if (dynamic_cast<NumberNode*>(squareNode->getOperand().get())) {
                    auto* numNode = dynamic_cast<NumberNode*>(squareNode->getOperand().get());
                    std::cout << "    - NumberNode: " << numNode->getValue() << std::endl;
                }
            }
        }
        
        AssemblyGenerator asmGen;
        std::string assembly = asmGen.generateHeader();
        assembly += asmGen.generateVariable("x");
        assembly += asmGen.generateCode(ast->toAssembly());
        
        // Write assembly to file
        std::ofstream outFile("output.asm");
        outFile << assembly;
        outFile.close();
        
        // Display the generated assembly code
        std::cout << "Generated Assembly Code:\n";
        std::cout << "------------------------\n";
        std::cout << assembly;
        std::cout << "------------------------\n\n";
        
        // Execute the AST
        int result = ast->evaluate();
        std::cout << "Result: " << result << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
} 