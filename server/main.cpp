// =====================================================
// main.cpp - Web server cho do an PBL2 "Quan Ly Khach San"
// Dung thu vien cpp-httplib (1 file header, MIT) lam HTTP server.
// - Phuc vu giao dien tinh trong thu muc web/
// - Cung cap REST API (JSON) tai /api/*
// Bien dich: xem Makefile hoac build.bat
// =====================================================

#include "../third_party/httplib.h"
#include "store.h"
#include "json.h"

#include <iostream>
#include <string>

using httplib::Request;
using httplib::Response;

// Lay tham so tu query string hoac body form-urlencoded
static string P(const Request& req, const string& ten, const string& macDinh = "") {
    if (req.has_param(ten.c_str())) return req.get_param_value(ten.c_str());
    return macDinh;
}
static long long PLL(const Request& req, const string& ten, long long macDinh = 0) {
    string v = P(req, ten);
    if (v.empty()) return macDinh;
    try { return std::stoll(v); } catch (...) { return macDinh; }
}
static int PInt(const Request& req, const string& ten, int macDinh = 0) {
    string v = P(req, ten);
    if (v.empty()) return macDinh;
    try { return std::stoi(v); } catch (...) { return macDinh; }
}

static void traJson(Response& res, const string& json, int status = 200) {
    res.status = status;
    res.set_content(json, "application/json; charset=utf-8");
}

// Bao boc KetQua thanh HTTP response
static void traKetQua(Response& res, const KetQua& kq) {
    if (kq.ok) {
        traJson(res, "{\"ok\":true,\"data\":" + (kq.jsonData.empty() ? "null" : kq.jsonData) + "}");
    } else {
        js::Obj ob;
        ob.kv("ok", "false").ks("thongBao", kq.thongBao);
        traJson(res, ob.str(), 400);
    }
}

int main(int argc, char** argv) {
    string thuMucData = "data";
    string thuMucWeb  = "web";
    int cong = 8080;
    if (argc > 1) cong = std::atoi(argv[1]);

    Store store(thuMucData);
    store.napTatCa();

    httplib::Server svr;

    // -------- Giao dien tinh --------
    if (!svr.set_mount_point("/", thuMucWeb)) {
        std::cerr << "Canh bao: khong mo duoc thu muc web/ (" << thuMucWeb << ")\n";
    }

    // ============ PHONG ============
    svr.Get("/api/rooms", [&](const Request&, Response& res) {
        traJson(res, store.dsPhongJson());
    });
    svr.Post("/api/rooms", [&](const Request& req, Response& res) {
        traKetQua(res, store.themPhong(P(req, "maPhong"), P(req, "loaiPhong"),
                                       PLL(req, "donGia"), PInt(req, "sucChua", 2),
                                       P(req, "moTa")));
    });
    svr.Post(R"(/api/rooms/([^/]+)/update)", [&](const Request& req, Response& res) {
        traKetQua(res, store.suaPhong(req.matches[1].str(), P(req, "loaiPhong"),
                                      PLL(req, "donGia", -1), PInt(req, "sucChua", -1),
                                      P(req, "trangThai"), P(req, "moTa")));
    });
    svr.Post(R"(/api/rooms/([^/]+)/delete)", [&](const Request& req, Response& res) {
        traKetQua(res, store.xoaPhong(req.matches[1].str()));
    });

    // ============ DICH VU ============
    svr.Get("/api/services", [&](const Request&, Response& res) {
        traJson(res, store.dsDichVuJson());
    });
    svr.Post("/api/services", [&](const Request& req, Response& res) {
        traKetQua(res, store.themDichVu(P(req, "tenDV"), P(req, "nhom"),
                                        PLL(req, "donGia"), PLL(req, "giaVon"),
                                        P(req, "donViTinh")));
    });
    svr.Post(R"(/api/services/([^/]+)/update)", [&](const Request& req, Response& res) {
        traKetQua(res, store.suaDichVu(req.matches[1].str(), P(req, "tenDV"), P(req, "nhom"),
                                       PLL(req, "donGia", -1), PLL(req, "giaVon", -1),
                                       P(req, "donViTinh")));
    });
    svr.Post(R"(/api/services/([^/]+)/delete)", [&](const Request& req, Response& res) {
        traKetQua(res, store.xoaDichVu(req.matches[1].str()));
    });

    // ============ DAT PHONG ============
    svr.Get("/api/bookings", [&](const Request& req, Response& res) {
        traJson(res, store.dsDatPhongJson(P(req, "trangThai")));
    });
    svr.Post("/api/bookings", [&](const Request& req, Response& res) {
        traKetQua(res, store.taoDatPhong(P(req, "tenKhach"), P(req, "cccd"), P(req, "sdt"),
                                         P(req, "maPhong"), P(req, "ngayNhan"),
                                         P(req, "ngayTra"), PLL(req, "tienCoc")));
    });
    svr.Post(R"(/api/bookings/([^/]+)/checkin)", [&](const Request& req, Response& res) {
        traKetQua(res, store.checkIn(req.matches[1].str()));
    });
    svr.Post(R"(/api/bookings/([^/]+)/checkout)", [&](const Request& req, Response& res) {
        traKetQua(res, store.checkOut(req.matches[1].str()));
    });
    svr.Post(R"(/api/bookings/([^/]+)/cancel)", [&](const Request& req, Response& res) {
        traKetQua(res, store.huyDat(req.matches[1].str()));
    });
    svr.Get(R"(/api/bookings/([^/]+)/bill)", [&](const Request& req, Response& res) {
        traJson(res, store.hoaDonJson(req.matches[1].str()));
    });
    svr.Get(R"(/api/bookings/([^/]+)/services)", [&](const Request& req, Response& res) {
        traJson(res, store.dsSuDungJson(req.matches[1].str()));
    });
    svr.Post(R"(/api/bookings/([^/]+)/services)", [&](const Request& req, Response& res) {
        traKetQua(res, store.themSuDung(req.matches[1].str(), P(req, "maDV"),
                                        PInt(req, "soLuong", 1), P(req, "ngay")));
    });
    svr.Post(R"(/api/service-orders/([^/]+)/delete)", [&](const Request& req, Response& res) {
        traKetQua(res, store.xoaSuDung(req.matches[1].str()));
    });

    // ============ THONG KE ============
    svr.Get("/api/stats", [&](const Request& req, Response& res) {
        traJson(res, store.thongKeJson(P(req, "tu"), P(req, "den")));
    });

    svr.set_error_handler([](const Request&, Response& res) {
        if (res.status == 404 && res.body.empty())
            traJson(res, "{\"ok\":false,\"thongBao\":\"Khong tim thay tai nguyen\"}", 404);
    });

    std::cout << "==============================================\n";
    std::cout << " QUAN LY KHACH SAN - WEB (PBL2)\n";
    std::cout << " Mo trinh duyet: http://localhost:" << cong << "\n";
    std::cout << " Nhan Ctrl+C de dung server\n";
    std::cout << "==============================================\n";

    if (!svr.listen("0.0.0.0", cong)) {
        std::cerr << "Loi: khong the lang nghe tai cong " << cong
                  << " (co the cong dang bi chiem).\n";
        return 1;
    }
    return 0;
}
