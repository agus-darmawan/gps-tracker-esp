#include <iostream>
#include <cmath>

using namespace std;

#define gravity 9.8
#define a_standart 3.0
#define T_standart 100
#define k 0.0693  // k = ln(2)/10 ≈ 0.0693

int rear_tier_force(int s_real, int h, int v_start, int v_end, int t) {
    double result = ((((v_end - v_start) / (double)t) + (gravity * h / s_real)) / a_standart * s_real);
    return round(std::abs(result));
}

int break_work(int s_real, int h, int v_start, int v_end, int t) {
    double result = ((((v_start - v_end) / (double)t) - (gravity * h / s_real)) / a_standart * s_real);
    return round(std::abs(result));
}

int count_s_oil(int s_real, int T_machine) {
    double result = s_real * std::exp(k * (T_machine - T_standart));
    return round(std::abs(result));
}

int main() {
    //Total jarak setiap komponen
    int s_rear_tier = 0;
    int s_front_tier = 0;
    int s_break = 0;
    int s_chain = 0;
    int s_oil = 0;
    int s_machine = 0; //Sama dengan jarak tempuh total

    int v_start = 0; 
    int t = 3;       

    cout << "=== SIMULASI BEBAN KOMPONEN MOTOR ===" << endl;
    cout << "Tekan Ctrl+C untuk menghentikan program" << endl;

    while (true) {
        int s_real, h, v_end, T_machine;
        cout << "\nMasukkan data baru:" << endl;
        cout << "Jarak tempuh (m): "; cin >> s_real;
        cout << "Ketinggian (h): "; cin >> h;
        cout << "Kecepatan akhir (v_end): "; cin >> v_end;
        cout << "Suhu mesin (T): "; cin >> T_machine;

        int delta_rear = s_real;
        int delta_front = s_real;
        int delta_break = 0;
        int delta_chain = s_real;

        // jalan datar
        if (h == 0) {
            if (v_end > v_start) { // akselerasi
                delta_rear = rear_tier_force(s_real, h, v_start, v_end, t);
                delta_chain = rear_tier_force(s_real, h, v_start, v_end, t);
            }
            else if (v_end < v_start) { // perlambatan
                int work = break_work(s_real, h, v_start, v_end, t);
                delta_rear = 0.3 * work;
                delta_front = 0.7 * work;
                delta_break = work;
                delta_chain = 0.3 * work;
            }
            
        }
        // menanjak
        else if (h > 0) {
            if (v_end >= v_start) { // akselerasi atau konstan
                delta_rear = rear_tier_force(s_real, h, v_start, v_end, t);
                delta_chain = rear_tier_force(s_real, h, v_start, v_end, t);
            }
            else { // perlambatan tanpa rem
                delta_rear = rear_tier_force(s_real, h, v_start, v_end, t);
                delta_chain = rear_tier_force(s_real, h, v_start, v_end, t);
            }
        }
        // menurun
        else {
            if (v_end > v_start) { // akselerasi
                delta_rear = rear_tier_force(s_real, h, v_start, v_end, t);
                delta_chain = rear_tier_force(s_real, h, v_start, v_end, t);
            }
            else { // perlambatan atau konstan -> pasti rem
                int work = break_work(s_real, h, v_start, v_end, t);
                delta_rear = 0.3 * work;
                delta_front = 0.7 * work;
                delta_break = work;
                delta_chain = 0.3 * work;
            }
        }

        // Update nilai kumulatif
        s_rear_tier += delta_rear;
        s_front_tier += delta_front;
        s_break += delta_break;
        s_chain += delta_chain;
        s_oil += count_s_oil(s_real, T_machine);
        s_machine += s_real;

        // Output hasil sementara
        cout << "\n--- HASIL KUMULATIF ---" << endl;
        cout << "S_rear_tier = " << s_rear_tier << endl;
        cout << "S_front_tier = " << s_front_tier << endl;
        cout << "S_break = " << s_break << endl;
        cout << "S_chain = " << s_chain << endl;
        cout << "S_oil = " << s_oil << endl;
        cout << "S_machine = " << s_machine << endl;

        // Update starting velocity
        v_start = v_end;
    }

    return 0;
}