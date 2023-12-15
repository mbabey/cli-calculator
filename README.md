### math - A Command Line Calculator
math implements a lexer and recursive descent parser to interpret and evaluate mathematical expressions.

### Installation
1. Clone the repository
2. cd into the repository directory
3. Run the build.sh script 
4. Remove unneeded files

```
git clone https://github.com/mbabey/cli-calculator
cd cli-calculator
./build.sh
cd ../
rm -rf cli-calculator
```

### Usage
math &lt;expression&gt;

### Description
Calculates and displays the result of the mathematical expression &lt;expression&gt;.
Input operands can be whole numbers or decimal numbers. A single decimal number operand,
regardless of the number of whole number operands, will make the result a decimal
number.

&lt;expression&gt; may contain spaces. If &lt;expression&gt; contains characters '/' or '*', it
must be wrapped in double quotes (eg: "&lt;expression&gt;").

Supported operations are:
- addition (**+**)
- subtraction (**-**)
- multiplication (**&ast;**)
- division (**/**)
- exponentiation (**^**)

### Example Usage
- `math 3+4`
- `math 3 + 4`
- `math 3*4`
- `math "3 * 4"`
- `math "((-20 - 2) * 4.5) / 11)"`
