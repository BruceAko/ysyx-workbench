module alu(
        input [2:0] func,
        input [3:0] a,
        input [3:0] b,
        output reg carry,
        output reg zero,
        output reg overflow,
        output reg [3:0] res
    );

    wire [3:0] xorb;

    always @ (*) begin
        case (func)
            3'b000: begin //加法
                {carry,res} = a + b;
                overflow = (a[3] == b[3]) && (res[3] != a[3]);
            end
            3'b001: begin //减法
                {carry,res} = a + xorb + 4'b1;
                overflow = (a[3] == xorb[3]) && (res[3] != a[3]); //按位取反后不先加1，计算是否溢出要用xorb来计算
            end
            3'b010: begin //取反
                res = ~a;
                carry = 0;
                overflow = 0;
            end
            3'b011: begin  //与
                res = a & b;
                carry = 0;
                overflow = 0;
            end
            3'b100: begin //或
                res = a | b;
                carry = 0;
                overflow = 0;
            end
            3'b101: begin //异或
                res = a ^ b;
                carry = 0;
                overflow = 0;
            end
            3'b110: begin //比较大小
                //带符号数的大小比较，可以用减法比较，即比较A、B两数大小时，首先B取反加一，然后与A相加。在不溢出时，结果的符号位为1则A小于B。如果减法溢出，则A和B原始符号一定不同。此时，如果结果符号位为0，说明A为负数，B为正数，B取反加一后为负，两者相加为正，所以A应小于B。在溢出时如果结果符号位为1，则B小于等于A。
                {carry,res} = a + xorb + 4'b1;
                overflow = (a[3] == xorb[3]) && (res[3] != a[3]);
                //A<B时out=1
                res = {3'b000,res[3] ^ overflow};
            end
            3'b111: begin //判断相等
                //全等可以用减法Zero输出判断
                {carry,res} = a + xorb + 4'b1;
                overflow = (a[3] == xorb[3]) && (res[3] != a[3]);
                //A==B时out=1
                res = {3'b000,~(| res)};
            end
        endcase
    end

    assign zero = ~(| res);
    assign xorb = b ^ 4'b1111;

endmodule //alu
