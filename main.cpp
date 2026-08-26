#include "hotel.h"

const string TEN_FILE = "data.txt";

// =====================================================
// Cac ham ho tro nhap lieu an toan (khong crash khi nguoi dung go sai kieu)
// =====================================================
int nhapSoNguyen(const string& prompt) {
    int x;
    while (true) {
        cout << prompt;
        cin >> x;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Vui long nhap so nguyen!\n";
            continue;
        }
        cin.ignore(10000, '\n');
        return x;
    }
}

string nhapChuoiKhongRong(const string& prompt) {
    string s;
    while (true) {
        cout << prompt;
        getline(cin, s);
        if (!s.empty()) return s;
        cout << "Khong duoc de trong!\n";
    }
}

Date nhapNgay(const string& nhan) {
    while (true) {
        cout << nhan << " (nhap: ngay thang nam, VD 15 8 2026): ";
        int d, m, y;
        cin >> d >> m >> y;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Sai dinh dang, vui long nhap lai.\n";
            continue;
        }
        cin.ignore(10000, '\n');
        Date ngay(d, m, y);
        if (!ngay.isValid()) {
            cout << "Ngay khong hop le, vui long nhap lai.\n";
            continue;
        }
        return ngay;
    }
}

// Nhap day du 1 khach hang, co validate tung truong va kiem tra trung ma hoa don.
KhachHang nhapKhachHang(const DanhSachKhachSan& ds) {
    KhachHang kh;

    while (true) {
        kh.maHD = nhapChuoiKhongRong("Ma hoa don: ");
        if (!validateMaHD(kh.maHD)) { cout << "Ma hoa don khong duoc rong.\n"; continue; }
        if (ds.tonTaiMaHD(kh.maHD)) { cout << "Ma hoa don da ton tai, vui long nhap ma khac.\n"; continue; }
        break;
    }

    kh.tenKhach = nhapChuoiKhongRong("Ten khach hang: ");

    while (true) {
        kh.cccd = nhapChuoiKhongRong("So CCCD/CMND: ");
        if (validateCCCD(kh.cccd)) break;
        cout << "CCCD khong hop le (phai gom 9 hoac 12 chu so).\n";
    }

    while (true) {
        kh.gioiTinh = nhapChuoiKhongRong("Gioi tinh (Nam/Nu/Khac): ");
        if (validateGioiTinh(kh.gioiTinh)) break;
        cout << "Gioi tinh khong hop le.\n";
    }

    while (true) {
        kh.soPhong = nhapChuoiKhongRong("So phong (VD A203): ");
        if (validateSoPhong(kh.soPhong)) break;
        cout << "So phong khong hop le (phai la 1 chu cai + cac chu so, VD A203).\n";
    }

    while (true) {
        kh.ngayDen = nhapNgay("Ngay den");
        kh.ngayDi = nhapNgay("Ngay di");
        if (kh.ngayDen < kh.ngayDi) break;
        cout << "Ngay di phai sau ngay den, vui long nhap lai ca hai ngay.\n";
    }

    kh.tinhToanLaiDuLieu();
    return kh;
}

void inMenu() {
    cout << "\n========== QUAN LY KHACH SAN ==========\n";
    cout << " 1. Doc du lieu tu file\n";
    cout << " 2. In danh sach\n";
    cout << " 3. Them vao dau\n";
    cout << " 4. Them vao cuoi\n";
    cout << " 5. Tim kiem\n";
    cout << " 6. Sap xep\n";
    cout << " 7. Chen tai vi tri bat ky\n";
    cout << " 8. Chen co thu tu (insertSorted)\n";
    cout << " 9. Xoa mot mau tin\n";
    cout << "10. Thong ke theo loai phong\n";
    cout << "11. Xoa khach ten Minh\n";
    cout << "12. Luu du lieu vao file\n";
    cout << "13. Thoat\n";
    cout << "========================================\n";
}

int main() {
    DanhSachKhachSan ds;
    bool daThayDoi = false;
    bool dangChay = true;

    cout << "CHUONG TRINH QUAN LY KHACH SAN\n";
    ds.readFile(TEN_FILE);   // tu dong doc file khi khoi dong chuong trinh

    while (dangChay) {
        inMenu();
        int chon = nhapSoNguyen("Chon chuc nang: ");

        switch (chon) {
        case 1: {
            ds.readFile(TEN_FILE);
            daThayDoi = false;
            break;
        }
        case 2: {
            ds.display();
            break;
        }
        case 3: {
            KhachHang kh = nhapKhachHang(ds);
            ds.addHead(kh);
            cout << "Da them vao dau danh sach.\n";
            daThayDoi = true;
            break;
        }
        case 4: {
            KhachHang kh = nhapKhachHang(ds);
            ds.addTail(kh);
            cout << "Da them vao cuoi danh sach.\n";
            daThayDoi = true;
            break;
        }
        case 5: {
            cout << "\n-- TIM KIEM --\n"
                 << "1. Theo ma hoa don\n2. Theo ten\n3. Theo CCCD\n"
                 << "4. Theo so phong\n5. Theo loai phong\n6. Quay lai\n";
            int c = nhapSoNguyen("Chon: ");
            if (c == 1) { string s = nhapChuoiKhongRong("Nhap ma hoa don: "); ds.searchByInvoice(s); }
            else if (c == 2) { string s = nhapChuoiKhongRong("Nhap ten (hoac 1 phan ten): "); ds.searchByName(s); }
            else if (c == 3) { string s = nhapChuoiKhongRong("Nhap CCCD: "); ds.searchByCCCD(s); }
            else if (c == 4) { string s = nhapChuoiKhongRong("Nhap so phong: "); ds.searchByRoom(s); }
            else if (c == 5) { string s = nhapChuoiKhongRong("Nhap loai phong (A/B/C...): "); ds.searchByRoomType(s[0]); }
            break;
        }
        case 6: {
            cout << "\n-- SAP XEP --\n"
                 << "1.Ma hoa don  2.Ten khach  3.So phong  4.Don gia\n"
                 << "5.Ngay den    6.Ngay di    7.So ngay o  8.Tien phong\n";
            int tc = nhapSoNguyen("Chon tieu chi (1-8): ");
            int td = nhapSoNguyen("Thu tu (1=Tang dan, 2=Giam dan): ");
            string tieuChi;
            switch (tc) {
                case 1: tieuChi = "mahd"; break;
                case 2: tieuChi = "ten"; break;
                case 3: tieuChi = "sophong"; break;
                case 4: tieuChi = "dongia"; break;
                case 5: tieuChi = "ngayden"; break;
                case 6: tieuChi = "ngaydi"; break;
                case 7: tieuChi = "songay"; break;
                case 8: tieuChi = "tienphong"; break;
                default: cout << "Lua chon khong hop le.\n"; break;
            }
            if (!tieuChi.empty()) {
                ds.sortBy(tieuChi, td == 1);
                cout << "Da sap xep. Danh sach sau khi sap xep:\n";
                ds.display();
                daThayDoi = true;
            }
            break;
        }
        case 7: {
            KhachHang kh = nhapKhachHang(ds);
            int pos = nhapSoNguyen("Nhap vi tri can chen (1 = dau danh sach): ");
            if (ds.insertAtPosition(kh, pos)) {
                cout << "Da chen thanh cong.\n";
                daThayDoi = true;
            }
            break;
        }
        case 8: {
            cout << "\n-- CHEN CO THU TU --\n"
                 << "(Luu y: danh sach phai dang duoc sap xep tang dan theo dung tieu chi nay truoc do)\n"
                 << "1. Theo tien phong\n2. Theo so phong\n3. Theo so ngay o\n";
            int c = nhapSoNguyen("Chon tieu chi: ");
            string tieuChi = (c == 2) ? "sophong" : (c == 3) ? "songay" : "tienphong";
            KhachHang kh = nhapKhachHang(ds);
            ds.insertSorted(kh, tieuChi);
            cout << "Da chen thanh cong, danh sach sau khi chen:\n";
            ds.display();
            daThayDoi = true;
            break;
        }
        case 9: {
            cout << "\n-- XOA MAU TIN --\n1. Theo ma hoa don\n2. Theo CCCD\n3. Quay lai\n";
            int c = nhapSoNguyen("Chon: ");
            if (c == 1 || c == 2) {
                string s = nhapChuoiKhongRong(c == 1 ? "Nhap ma hoa don can xoa: " : "Nhap CCCD can xoa: ");
                string xn = nhapChuoiKhongRong("Ban co chac chan muon xoa? (Y/N): ");
                if (xn == "Y" || xn == "y") {
                    bool ok = (c == 1) ? ds.deleteByInvoice(s) : ds.deleteByCCCD(s);
                    if (ok) { cout << "Da xoa thanh cong.\n"; daThayDoi = true; }
                    else cout << "Khong tim thay record can xoa.\n";
                } else {
                    cout << "Da huy thao tac xoa.\n";
                }
            }
            break;
        }
        case 10: {
            ds.statisticsByRoomType();
            break;
        }
        case 11: {
            string xn = nhapChuoiKhongRong("Ban co chac muon xoa toan bo khach ten Minh? (Y/N): ");
            if (xn == "Y" || xn == "y") {
                int soLuong = ds.deleteCustomersNamedMinh();
                cout << "Da xoa " << soLuong << " hoa don co ten chua \"Minh\".\n";
                if (soLuong > 0) daThayDoi = true;
            } else {
                cout << "Da huy thao tac xoa.\n";
            }
            break;
        }
        case 12: {
            ds.writeFile(TEN_FILE);
            daThayDoi = false;
            break;
        }
        case 13: {
            if (daThayDoi) {
                string xn = nhapChuoiKhongRong("Ban co muon luu thay doi truoc khi thoat? (Y/N): ");
                if (xn == "Y" || xn == "y") ds.writeFile(TEN_FILE);
            }
            cout << "Tam biet!\n";
            dangChay = false;
            break;
        }
        default:
            cout << "Lua chon khong hop le, vui long chon lai.\n";
        }
    }

    // ds ra khoi pham vi -> ~DanhSachKhachSan() tu dong chay -> freeList() tu chay, khong ro ri bo nho
    return 0;
}
