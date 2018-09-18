#include <cstdio>
#include <string>

#define B(a, b) (((a) >> (b)) & 1)
#define N(a, b, c) (!((a) && (b) && (c)))

int simulate(int bitmask, int inputs) {
    int a = B(inputs, 0);
    int b = B(inputs, 1);
    int c = B(inputs, 2);

    int g0  = N(b,  B(bitmask, 0), 1);
    int g1  = N(b,  B(bitmask, 1), g0);
    int g2  = N(g0, B(bitmask, 2), 1);
    int g3  = N(g1, B(bitmask, 3), g2);
    int g4  = N(a,  B(bitmask, 4), g3);
    int g5  = N(a,  B(bitmask, 5), g4);
    int g6  = N(g4, B(bitmask, 6), g3);
    int g7  = N(g5, B(bitmask, 7), g6);
    int g8  = N(c,  B(bitmask, 8), g7);
    int g9  = N(g7, B(bitmask, 9), g8);
    int g10 = N(g8, B(bitmask, 10), c);
    int g11 = N(g9, B(bitmask, 11), g10);

    /*
    printf("%d %d %d\n", a, b, c);
    printf("%d %d %d %d\n", g0, g1, g2, g3);
    printf("%d %d %d %d\n", g4, g5, g6, g7);
    printf("%d %d %d %d\n", g8, g9, g10, g11);
    */

    return g11;
}

void bin3(int i) {
    printf("%d%d%d", B(i, 0), B(i, 1), B(i, 2));
}

std::string bin12(int i) {
    char buf[20];
    sprintf(buf, "%d%d%d%d%d%d%d%d%d%d%d%d", 
            B(i, 0), B(i, 1), B(i, 2), B(i, 3),
            B(i, 4), B(i, 5), B(i, 6), B(i, 7),
            B(i, 8), B(i, 9), B(i,10), B(i,11));
    return buf;
}

std::string bin8(int i) {
    char buf[10];
    sprintf(buf, "%d%d%d%d%d%d%d%d", 
            B(i, 0), B(i, 1), B(i, 2), B(i, 3),
            B(i, 4), B(i, 5), B(i, 6), B(i, 7));
    return buf;
}

std::string bin4(int i) {
    char buf[10];
    sprintf(buf, "%d%d%d%d", 
            B(i, 0), B(i, 1), B(i, 2), B(i, 3));
    return buf;
}

int check(int bitmask) {
    int res = 0;
    for (int i = 0; i < 8; i++) {
        res |= simulate(bitmask, i) << i;
    }
    return res;
}

std::string classify(int result) {
    if (result == 0b10010110) return "ADD";
    if (result == 0b01101001) return "SUB";

    // In other cases, carry should not matter:
    for (int i = 0; i < 4; i++) {
        if (B(result, i) != B(result, i | 4))
            return "   -";
    }

    auto bin = bin4(result);

    if (bin == "0000") return "CLR";
    if (bin == "1111") return "SET";

    if (bin == "0010") return "~imply";
    if (bin == "1101") return "imply";
    if (bin == "0100") return "~imply2";

    if (bin == "0001") return "AND";

    if (bin == "1100") return "~B";
    if (bin == "1010") return "~A";
    if (bin == "0011") return "B";
    if (bin == "0101") return "A";

    if (bin == "1001") return "XNOR";
    if (bin == "0110") return "XOR";

    if (bin == "0111") return "OR";
    if (bin == "1000") return "NOR";


    return " " + bin;
}

int sums[256];

int main() {
    for (int i = 0; i < (1<<12); i++) {
        sums[check(i)]++;
    }
    for (int i = 0; i < 256; i++) {
        if (!sums[i]) continue;
        auto s = classify(i);
        if (s == "   -") continue;
        printf("%s\t %d\n", s.c_str(), sums[i]);
    }
    printf("012345678901\n");
    for (int i = 0; i < (1<<12); i++) {
        auto s = classify(check(i));
        // if (s == "XOR" || s == "ADD" || s == "OR" || s == "SUB" || s == "AND") {
        if (s == "B") {
            printf("%s %s\n", bin12(i).c_str(), s.c_str());
        }
    }
}
