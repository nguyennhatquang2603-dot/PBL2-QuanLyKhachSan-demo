#include "hotel.h"
#include <iomanip>
#include <sstream>
#include <cctype>
#include <fstream>
#include <cstdio>

// =====================================================
// Ham phu dung chung
// =====================================================
bool isLeapYear(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

// =====================================================
// class Date
// =====================================================
bool Date::isValid() const {
    if (year < 1900 || year > 2100) return false;
    if (month < 1 || month > 12) return false;

    static int soNgayThang[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int soNgayToiDa = soNgayThang[month - 1];
    if (month == 2 && isLeapYear(year)) soNgayToiDa = 29;

    if (day < 1 || day > soNgayToiDa) return false;
    return true;
}

// Doi ngay/thang/nam thanh 1 con so duy nhat, tang dan theo thoi gian.
// Muc dich: de tinh "ngayDi - ngayDen" va de so sanh 2 ngay bang phep tru/so sanh so
// thay vi phai so sanh tung phan tu (nam, roi thang, roi ngay) rat rom ra.
long Date::toDayNumber() const {
    static int soNgayThang[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    // so ngay tinh tu nam 0 den dau nam "year" (cong thuc chuan, tinh ca nam nhuan)
    long tong = (long)year * 365 + year / 4 - year / 100 + year / 400;

    for (int i = 0; i < month - 1; i++) {
        tong += soNgayThang[i];
    }
    if (month > 2 && isLeapYear(year)) tong++;   // da qua thang 2 trong nam nhuan

    tong += day;
    return tong;
}

string Date::toString() const {
    ostringstream oss;
    oss << setfill('0') << setw(2) << day << "/"
        << setfill('0') << setw(2) << month << "/" << year;
    return oss.str();
}

bool Date::operator<(const Date& other) const {
    return toDayNumber() < other.toDayNumber();
}

// =====================================================
// class KhachHang
// =====================================================

// Tinh lai toan bo du lieu suy ra tu du lieu goc (soPhong, ngayDen, ngayDi).
// Ham nay PHAI duoc goi lai moi khi du lieu goc thay doi (nhap moi, doc tu file...)
// de dam bao du lieu suy ra luon dung, khong bao gio bi "lech" so voi du lieu goc.
void KhachHang::tinhToanLaiDuLieu() {
    loaiPhong = soPhong.empty() ? ' ' : (char)toupper(soPhong[0]);

    if (loaiPhong == 'A') donGia = 400;
    else if (loaiPhong == 'B') donGia = 300;
    else donGia = 200;

    soNgay = (int)(ngayDi.toDayNumber() - ngayDen.toDayNumber());
    if (soNgay < 0) soNgay = 0;   // an toan; viec bat loi "ngay di truoc ngay den" thuc hien o buoc validate khi nhap

    tienPhong = (long)donGia * soNgay;
}

void KhachHang::hienThiDong(int stt) const {
    cout << left
         << setw(4)  << stt
         << setw(12) << maHD
         << setw(20) << tenKhach
         << setw(14) << cccd
         << setw(6)  << gioiTinh
         << setw(8)  << soPhong
         << setw(6)  << loaiPhong
         << setw(8)  << donGia
         << setw(12) << ngayDen.toString()
         << setw(12) << ngayDi.toString()
         << setw(8)  << soNgay
         << setw(10) << tienPhong
         << "\n";
}

// =====================================================
// class DanhSachKhachSan - Phase 2: thao tac co ban tren linked list
// =====================================================

// Duyet toan bo danh sach, delete tung node, cuoi cung dua head ve nullptr.
// Duoc goi tu destructor -> chuong trinh ket thuc se tu dong khong con memory leak.
void DanhSachKhachSan::freeList() {
    Node* cur = head;
    while (cur != nullptr) {
        Node* tam = cur;
        cur = cur->next;
        delete tam;
    }
    head = nullptr;
}

int DanhSachKhachSan::demSoLuong() const {
    int dem = 0;
    Node* cur = head;
    while (cur != nullptr) {
        dem++;
        cur = cur->next;
    }
    return dem;
}

// Them vao DAU danh sach: node moi tro toi head cu, roi head duoc cap nhat tro toi node moi.
void DanhSachKhachSan::addHead(const KhachHang& kh) {
    Node* newNode = new Node(kh);
    newNode->data.tinhToanLaiDuLieu();
    newNode->next = head;
    head = newNode;
}

// Them vao CUOI danh sach: duyet toi node cuoi cung (next == nullptr) roi noi node moi vao sau no.
void DanhSachKhachSan::addTail(const KhachHang& kh) {
    Node* newNode = new Node(kh);
    newNode->data.tinhToanLaiDuLieu();

    if (head == nullptr) {
        head = newNode;
        return;
    }

    Node* cur = head;
    while (cur->next != nullptr) {
        cur = cur->next;
    }
    cur->next = newNode;
}

void DanhSachKhachSan::display() const {
    if (head == nullptr) {
        cout << "Danh sach rong.\n";
        return;
    }

    cout << left
         << setw(4)  << "STT"
         << setw(12) << "MaHD"
         << setw(20) << "TenKhach"
         << setw(14) << "CCCD"
         << setw(6)  << "GT"
         << setw(8)  << "Phong"
         << setw(6)  << "Loai"
         << setw(8)  << "DonGia"
         << setw(12) << "NgayDen"
         << setw(12) << "NgayDi"
         << setw(8)  << "SoNgay"
         << setw(10) << "TienPhong"
         << "\n";
    cout << string(116, '-') << "\n";

    int stt = 1;
    Node* cur = head;
    while (cur != nullptr) {
        cur->data.hienThiDong(stt++);
        cur = cur->next;
    }
}

// =====================================================
// Ham phu noi bo (khong khai bao trong header)
// =====================================================
static string toLowerStr(const string& s) {
    string ketQua = s;
    for (size_t i = 0; i < ketQua.size(); i++) {
        ketQua[i] = (char)tolower((unsigned char)ketQua[i]);
    }
    return ketQua;
}

static void inTieuDeBang() {
    cout << left
         << setw(4)  << "STT"
         << setw(12) << "MaHD"
         << setw(20) << "TenKhach"
         << setw(14) << "CCCD"
         << setw(6)  << "GT"
         << setw(8)  << "Phong"
         << setw(6)  << "Loai"
         << setw(8)  << "DonGia"
         << setw(12) << "NgayDen"
         << setw(12) << "NgayDi"
         << setw(8)  << "SoNgay"
         << setw(10) << "TienPhong"
         << "\n";
    cout << string(116, '-') << "\n";
}

// so sanh 2 KhachHang theo 1 tieu chi, tra ve -1 (a<b), 0 (bang), 1 (a>b)
static int soSanh(const KhachHang& a, const KhachHang& b, const string& tieuChi) {
    if (tieuChi == "mahd") return a.maHD.compare(b.maHD);
    if (tieuChi == "ten") return a.tenKhach.compare(b.tenKhach);
    if (tieuChi == "sophong") return a.soPhong.compare(b.soPhong);
    if (tieuChi == "dongia") return (a.donGia < b.donGia) ? -1 : (a.donGia > b.donGia ? 1 : 0);
    if (tieuChi == "ngayden") return (a.ngayDen < b.ngayDen) ? -1 : (b.ngayDen < a.ngayDen ? 1 : 0);
    if (tieuChi == "ngaydi")  return (a.ngayDi  < b.ngayDi)  ? -1 : (b.ngayDi  < a.ngayDi  ? 1 : 0);
    if (tieuChi == "songay") return (a.soNgay < b.soNgay) ? -1 : (a.soNgay > b.soNgay ? 1 : 0);
    // mac dinh "tienphong"
    return (a.tienPhong < b.tienPhong) ? -1 : (a.tienPhong > b.tienPhong ? 1 : 0);
}

// =====================================================
// DanhSachKhachSan - Them (Phase 4 + Phase 7)
// =====================================================

// Chen vao vi tri "pos" (danh cho nguoi dung, dem tu 1).
// pos = 1        -> chen dau
// pos = n+1       -> chen cuoi (n = so phan tu hien co)
// 1 < pos <= n    -> chen vao giua: tim node dung TRUOC vi tri can chen,
//                    tuc node thu (pos-1), roi noi node moi vao giua no va node ke tiep.
bool DanhSachKhachSan::insertAtPosition(const KhachHang& kh, int pos) {
    int n = demSoLuong();
    if (pos < 1 || pos > n + 1) {
        cout << "Vi tri khong hop le (danh sach hien co " << n
             << " phan tu, vi tri hop le tu 1 den " << (n + 1) << ").\n";
        return false;
    }

    if (pos == 1) {
        addHead(kh);
        return true;
    }
    if (pos == n + 1) {
        addTail(kh);
        return true;
    }

    Node* cur = head;
    for (int i = 1; i < pos - 1; i++) {
        cur = cur->next;
    }
    Node* newNode = new Node(kh);
    newNode->data.tinhToanLaiDuLieu();
    newNode->next = cur->next;
    cur->next = newNode;
    return true;
}

// Chen giu danh sach van co thu tu tang dan theo tieuChi ("tienphong" / "sophong" / "songay").
// Logic: duyet tu dau, dung lai ngay TRUOC phan tu dau tien co gia tri LON HON kh,
// roi chen kh vao vi tri do. Neu danh sach dang rong hoac phan tu dau da > kh thi chen len dau.
bool DanhSachKhachSan::insertSorted(const KhachHang& khIn, const string& tieuChi) {
    KhachHang kh = khIn;
    kh.tinhToanLaiDuLieu();
    Node* newNode = new Node(kh);

    if (head == nullptr || soSanh(head->data, kh, tieuChi) > 0) {
        newNode->next = head;
        head = newNode;
        return true;
    }

    Node* cur = head;
    while (cur->next != nullptr && soSanh(cur->next->data, kh, tieuChi) <= 0) {
        cur = cur->next;
    }
    newNode->next = cur->next;
    cur->next = newNode;
    return true;
}

// =====================================================
// DanhSachKhachSan - Xoa (Phase 4 + Phase 8)
// =====================================================
bool DanhSachKhachSan::deleteByInvoice(const string& maHD) {
    if (head == nullptr) return false;

    if (head->data.maHD == maHD) {
        Node* tam = head;
        head = head->next;
        delete tam;
        return true;
    }

    Node* cur = head;
    while (cur->next != nullptr && cur->next->data.maHD != maHD) {
        cur = cur->next;
    }
    if (cur->next == nullptr) return false;   // khong tim thay

    Node* tam = cur->next;
    cur->next = tam->next;
    delete tam;
    return true;
}

bool DanhSachKhachSan::deleteByCCCD(const string& cccd) {
    if (head == nullptr) return false;

    if (head->data.cccd == cccd) {
        Node* tam = head;
        head = head->next;
        delete tam;
        return true;
    }

    Node* cur = head;
    while (cur->next != nullptr && cur->next->data.cccd != cccd) {
        cur = cur->next;
    }
    if (cur->next == nullptr) return false;

    Node* tam = cur->next;
    cur->next = tam->next;
    delete tam;
    return true;
}

// Xoa tat ca khach co ten CHUA chu "minh" (khong phan biet hoa/thuong).
// Xu ly rieng phan dau danh sach bang vong while (vi head co the bi xoa lien tiep
// nhieu lan, ke ca truong hop TOAN BO danh sach deu la Minh -> head se ve nullptr).
// Sau do duyet phan con lai bang con tro "cur" luon dung truoc node dang xet.
int DanhSachKhachSan::deleteCustomersNamedMinh() {
    int soLuongXoa = 0;

    while (head != nullptr && toLowerStr(head->data.tenKhach).find("minh") != string::npos) {
        Node* tam = head;
        head = head->next;
        delete tam;
        soLuongXoa++;
    }
    if (head == nullptr) return soLuongXoa;

    Node* cur = head;
    while (cur->next != nullptr) {
        if (toLowerStr(cur->next->data.tenKhach).find("minh") != string::npos) {
            Node* tam = cur->next;
            cur->next = tam->next;
            delete tam;
            soLuongXoa++;
        } else {
            cur = cur->next;
        }
    }
    return soLuongXoa;
}

bool DanhSachKhachSan::tonTaiMaHD(const string& maHD) const {
    Node* cur = head;
    while (cur != nullptr) {
        if (cur->data.maHD == maHD) return true;
        cur = cur->next;
    }
    return false;
}

// =====================================================
// DanhSachKhachSan - Tim kiem (Phase 5)
// =====================================================
void DanhSachKhachSan::searchByInvoice(const string& maHD) const {
    Node* cur = head;
    while (cur != nullptr) {
        if (cur->data.maHD == maHD) {
            cout << "Tim thay:\n";
            inTieuDeBang();
            cur->data.hienThiDong(1);
            return;
        }
        cur = cur->next;
    }
    cout << "Khong tim thay hoa don co ma \"" << maHD << "\".\n";
}

void DanhSachKhachSan::searchByCCCD(const string& cccd) const {
    Node* cur = head;
    while (cur != nullptr) {
        if (cur->data.cccd == cccd) {
            cout << "Tim thay:\n";
            inTieuDeBang();
            cur->data.hienThiDong(1);
            return;
        }
        cur = cur->next;
    }
    cout << "Khong tim thay khach hang co CCCD \"" << cccd << "\".\n";
}

void DanhSachKhachSan::searchByRoom(const string& soPhong) const {
    bool timThay = false;
    int stt = 1;
    Node* cur = head;
    while (cur != nullptr) {
        if (cur->data.soPhong == soPhong) {
            if (!timThay) { cout << "Tim thay:\n"; inTieuDeBang(); }
            cur->data.hienThiDong(stt++);
            timThay = true;
        }
        cur = cur->next;
    }
    if (!timThay) cout << "Khong tim thay khach o phong \"" << soPhong << "\".\n";
}

void DanhSachKhachSan::searchByRoomType(char loai) const {
    loai = (char)toupper(loai);
    bool timThay = false;
    int stt = 1;
    Node* cur = head;
    while (cur != nullptr) {
        if (cur->data.loaiPhong == loai) {
            if (!timThay) { cout << "Tim thay:\n"; inTieuDeBang(); }
            cur->data.hienThiDong(stt++);
            timThay = true;
        }
        cur = cur->next;
    }
    if (!timThay) cout << "Khong tim thay khach o loai phong \"" << loai << "\".\n";
}

void DanhSachKhachSan::searchByName(const string& ten) const {
    string tenTimLower = toLowerStr(ten);
    bool timThay = false;
    int stt = 1;
    Node* cur = head;
    while (cur != nullptr) {
        if (toLowerStr(cur->data.tenKhach).find(tenTimLower) != string::npos) {
            if (!timThay) { cout << "Tim thay:\n"; inTieuDeBang(); }
            cur->data.hienThiDong(stt++);
            timThay = true;
        }
        cur = cur->next;
    }
    if (!timThay) cout << "Khong tim thay khach ten \"" << ten << "\".\n";
}

// =====================================================
// DanhSachKhachSan - Sap xep (Phase 6)
// Bubble sort: duyet nhieu luot, moi luot so sanh 2 node ke nhau,
// neu sai thu tu thi HOAN DOI DU LIEU (data) giua 2 node thay vi noi lai con tro next.
// Cach nay don gian, de giai thich, tranh sai sot khi phai tu viet lai lien ket next.
// =====================================================
void DanhSachKhachSan::sortBy(const string& tieuChi, bool tangDan) {
    if (head == nullptr || head->next == nullptr) return;

    bool daDoiCho = true;
    while (daDoiCho) {
        daDoiCho = false;
        Node* cur = head;
        while (cur->next != nullptr) {
            int cmp = soSanh(cur->data, cur->next->data, tieuChi);
            bool phaiDoiCho = tangDan ? (cmp > 0) : (cmp < 0);
            if (phaiDoiCho) {
                KhachHang tam = cur->data;
                cur->data = cur->next->data;
                cur->next->data = tam;
                daDoiCho = true;
            }
            cur = cur->next;
        }
    }
}

// =====================================================
// DanhSachKhachSan - Thong ke theo loai phong (Phase 8)
// Dung mang 26 phan tu (ung voi 'A'..'Z') de cong don tien phong theo tung loai,
// khong hard-code chi A/B/C nen ho tro moi loai phong xuat hien trong danh sach.
// =====================================================
void DanhSachKhachSan::statisticsByRoomType() const {
    long tongTien[26] = {0};
    bool xuatHien[26] = {false};

    Node* cur = head;
    while (cur != nullptr) {
        char loai = cur->data.loaiPhong;
        if (loai >= 'A' && loai <= 'Z') {
            int idx = loai - 'A';
            tongTien[idx] += cur->data.tienPhong;
            xuatHien[idx] = true;
        }
        cur = cur->next;
    }

    cout << "\n=== THONG KE THEO LOAI PHONG ===\n";
    cout << left << setw(12) << "Loai phong" << setw(15) << "Tong tien" << "\n";
    cout << string(27, '-') << "\n";
    bool coDuLieu = false;
    for (int i = 0; i < 26; i++) {
        if (xuatHien[i]) {
            cout << left << setw(12) << string(1, (char)('A' + i)) << setw(15) << tongTien[i] << "\n";
            coDuLieu = true;
        }
    }
    if (!coDuLieu) cout << "Danh sach rong, khong co du lieu thong ke.\n";
}

// =====================================================
// DanhSachKhachSan - Doc/ghi file (Phase 3)
// Format 1 dong / 1 record, cac truong cach nhau boi dau '|':
//   MaHD|TenKhach|CCCD|GioiTinh|SoPhong|NgayDen(dd/mm/yyyy)|NgayDi(dd/mm/yyyy)
// CHI luu du lieu GOC. Cac truong suy ra (loaiPhong, donGia, soNgay, tienPhong)
// KHONG duoc ghi xuong file - se duoc tinh lai bang tinhToanLaiDuLieu() ngay sau khi doc.
// =====================================================
bool DanhSachKhachSan::readFile(const string& filename) {
    ifstream fin(filename.c_str());
    if (!fin.is_open()) {
        cout << "Khong tim thay file \"" << filename << "\". Bat dau voi danh sach rong.\n";
        return false;
    }

    freeList();   // xoa sach du lieu cu trong bo nho truoc khi nap du lieu moi tu file

    string line;
    int soLoi = 0;
    while (getline(fin, line)) {
        if (line.empty()) continue;

        string field[7];
        int idx = 0;
        size_t start = 0;
        for (size_t i = 0; i <= line.size() && idx < 7; i++) {
            if (i == line.size() || line[i] == '|') {
                field[idx++] = line.substr(start, i - start);
                start = i + 1;
            }
        }
        if (idx < 7) { soLoi++; continue; }

        KhachHang kh;
        kh.maHD = field[0];
        kh.tenKhach = field[1];
        kh.cccd = field[2];
        kh.gioiTinh = field[3];
        kh.soPhong = field[4];

        int d, m, y;
        if (sscanf(field[5].c_str(), "%d/%d/%d", &d, &m, &y) != 3) { soLoi++; continue; }
        kh.ngayDen = Date(d, m, y);
        if (sscanf(field[6].c_str(), "%d/%d/%d", &d, &m, &y) != 3) { soLoi++; continue; }
        kh.ngayDi = Date(d, m, y);

        kh.tinhToanLaiDuLieu();
        addTail(kh);
    }
    fin.close();

    cout << "Da doc " << demSoLuong() << " record tu file \"" << filename << "\".";
    if (soLoi > 0) cout << " (" << soLoi << " dong loi bi bo qua)";
    cout << "\n";
    return true;
}

bool DanhSachKhachSan::writeFile(const string& filename) const {
    ofstream fout(filename.c_str());
    if (!fout.is_open()) {
        cout << "Loi: khong the mo file \"" << filename << "\" de ghi. Du lieu trong bo nho van con nguyen.\n";
        return false;
    }

    Node* cur = head;
    while (cur != nullptr) {
        const KhachHang& kh = cur->data;
        fout << kh.maHD << "|" << kh.tenKhach << "|" << kh.cccd << "|"
             << kh.gioiTinh << "|" << kh.soPhong << "|"
             << kh.ngayDen.toString() << "|" << kh.ngayDi.toString() << "\n";
        cur = cur->next;
    }
    fout.close();

    cout << "Da luu " << demSoLuong() << " record vao file \"" << filename << "\".\n";
    return true;
}

// =====================================================
// Validation (Phase 9)
// =====================================================
bool validateMaHD(const string& s) {
    return !s.empty() && s.size() <= 10;   // gioi han do dai de bang hien thi khong bi le cot
}

bool validateCCCD(const string& s) {
    if (s.size() != 9 && s.size() != 12) return false;   // CMND 9 so hoac CCCD 12 so
    for (size_t i = 0; i < s.size(); i++) {
        if (!isdigit((unsigned char)s[i])) return false;
    }
    return true;
}

bool validateSoPhong(const string& s) {
    if (s.size() < 2) return false;
    if (!isalpha((unsigned char)s[0])) return false;
    for (size_t i = 1; i < s.size(); i++) {
        if (!isdigit((unsigned char)s[i])) return false;
    }
    return true;
}

bool validateGioiTinh(const string& s) {
    return s == "Nam" || s == "Nu" || s == "Khac";
}
