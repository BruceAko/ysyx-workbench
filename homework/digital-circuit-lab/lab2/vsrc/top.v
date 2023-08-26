module encode83(x,en,y,h);
    input [7:0] x;
    input en;
    output reg [3:0] y;
    output [6:0] h;
    integer i;

    always @ (*) begin
        if(en == 1) begin
            if(x == 8'b00000000)
                y = 0;
            else begin
                y[3] = 1;
                for (i = 0; i <= 7 ; i++) begin
                    if (x[i] == 1)
                        y[2:0] = i[2:0];
                end
            end
        end
        else
            y = 0;
    end

    bcd7seg i1(.en(y[3]), .b({1'b0,y[2:0]}), .h(h));

endmodule //encode83

module bcd7seg(
        input  en,
        input  [3:0] b,
        output reg [6:0] h
    );

    always @ (*) begin
        if (en == 1) begin
            case (b)
                4'd0:
                    h = 7'b0000001;
                4'd1:
                    h = 7'b1001111;
                4'd2:
                    h = 7'b0010010;
                4'd3:
                    h = 7'b0000110;
                4'd4:
                    h = 7'b1001100;
                4'd5:
                    h = 7'b0100100;
                4'd6:
                    h = 7'b0100000;
                4'd7:
                    h = 7'b0001111;
                4'd8:
                    h = 7'b0000000;
                4'd9:
                    h = 7'b0000001;
                default:
                    h = 7'b0000000;
            endcase
        end
        else begin
            h = 7'b0000000;
        end
    end
endmodule
