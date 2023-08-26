module MuxKeyInternal #(NR_KEY = 2, KEY_LEN = 1, DATA_LEN = 1, HAS_DEFAULT = 0) (
        output reg [DATA_LEN-1:0] out,
        input [KEY_LEN-1:0] key,
        input [DATA_LEN-1:0] default_out,
        input [NR_KEY*(KEY_LEN + DATA_LEN)-1:0] lut
    );

    localparam PAIR_LEN = KEY_LEN + DATA_LEN;
    wire [PAIR_LEN-1:0] pair_list [NR_KEY-1:0];
    wire [KEY_LEN-1:0] key_list [NR_KEY-1:0];
    wire [DATA_LEN-1:0] data_list [NR_KEY-1:0];

    generate
        for (genvar n = 0; n < NR_KEY; n = n + 1) begin
            assign pair_list[n] = lut[PAIR_LEN*(n+1)-1 : PAIR_LEN*n];
            assign data_list[n] = pair_list[n][DATA_LEN-1:0];
            assign key_list[n]  = pair_list[n][PAIR_LEN-1:DATA_LEN];
        end
    endgenerate

    reg [DATA_LEN-1 : 0] lut_out;
    reg hit;
    integer i;
    always @(*) begin
        lut_out = 0;
        hit = 0;
        for (i = 0; i < NR_KEY; i = i + 1) begin
            lut_out = lut_out | ({DATA_LEN{key == key_list[i]}} & data_list[i]);
            hit = hit | (key == key_list[i]);
        end
        if (!HAS_DEFAULT)
            out = lut_out;
        else
            out = (hit ? lut_out : default_out);
    end

endmodule

module MuxKey #(NR_KEY = 2, KEY_LEN = 1, DATA_LEN = 1) (
        output [DATA_LEN-1:0] out,
        input [KEY_LEN-1:0] key,
        input [NR_KEY*(KEY_LEN + DATA_LEN)-1:0] lut
    );
    MuxKeyInternal #(NR_KEY, KEY_LEN, DATA_LEN, 0) i0 (out, key, {DATA_LEN{1'b0}}, lut);
endmodule

module MuxKeyWithDefault #(NR_KEY = 2, KEY_LEN = 1, DATA_LEN = 1) (
        output [DATA_LEN-1:0] out,
        input [KEY_LEN-1:0] key,
        input [DATA_LEN-1:0] default_out,
        input [NR_KEY*(KEY_LEN + DATA_LEN)-1:0] lut
    );
    MuxKeyInternal #(NR_KEY, KEY_LEN, DATA_LEN, 1) i0 (out, key, default_out, lut);
endmodule

module mux2bit41(X0,X1,X2,X3,Y,F);
    input  [1:0] X0;
    input  [1:0] X1;
    input  [1:0] X2;
    input  [1:0] X3;
    input  [1:0] Y;
    output reg [1:0] F;
    always @ (*)
    case (Y)
        0:
            F=X0;
        1:
            F=X1;
        2:
            F=X2;
        3:
            F=X3;
    endcase
endmodule

module mux2bit41b(X0,X1,X2,X3,Y,F);
    input  [1:0] X0;
    input  [1:0] X1;
    input  [1:0] X2;
    input  [1:0] X3;
    input  [1:0] Y;
    output [1:0] F;
    MuxKeyWithDefault #(4, 2, 2) i0 (F, Y, 2'b00, {
                                         2'b00, X0,
                                         2'b01, X1,
                                         2'b10, X2,
                                         2'b11, X3
                                     });
endmodule
