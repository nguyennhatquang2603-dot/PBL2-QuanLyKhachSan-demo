/* =====================================================
   app.js - logic giao dien Quan Ly Khach San (PBL2)
   Goi REST API tai /api/*, du lieu gui len dang form-urlencoded.
   ===================================================== */

// ---------- Tien ich ----------
const $ = (sel, el = document) => el.querySelector(sel);
const $$ = (sel, el = document) => [...el.querySelectorAll(sel)];

const nf = new Intl.NumberFormat("vi-VN");
const money = (n) => nf.format(Number(n) || 0) + " ₫";

function el(tag, attrs = {}, ...kids) {
  const n = document.createElement(tag);
  for (const [k, v] of Object.entries(attrs)) {
    if (k === "class") n.className = v;
    else if (k === "html") n.innerHTML = v;
    else if (k.startsWith("on")) n.addEventListener(k.slice(2), v);
    else if (v !== null && v !== undefined) n.setAttribute(k, v);
  }
  for (const kid of kids.flat()) {
    if (kid == null) continue;
    n.append(kid.nodeType ? kid : document.createTextNode(kid));
  }
  return n;
}

async function api(path, method = "GET", params = null) {
  const opt = { method };
  if (params) {
    opt.headers = { "Content-Type": "application/x-www-form-urlencoded; charset=UTF-8" };
    opt.body = new URLSearchParams(params).toString();
  }
  const res = await fetch("/api" + path, opt);
  const text = await res.text();
  let data;
  try { data = text ? JSON.parse(text) : null; } catch { data = null; }
  if (!res.ok || (data && data.ok === false)) {
    const msg = (data && data.thongBao) || ("Loi HTTP " + res.status);
    throw new Error(msg);
  }
  return data;
}

let toastTimer;
function toast(msg, type = "") {
  const t = $("#toast");
  t.textContent = msg;
  t.className = "toast " + type;
  clearTimeout(toastTimer);
  toastTimer = setTimeout(() => t.classList.add("hidden"), 3200);
}

// ---------- Modal ----------
function openModal(title, bodyNode, wide = false) {
  $("#modal-title").textContent = title;
  const body = $("#modal-body");
  body.innerHTML = "";
  body.append(bodyNode);
  $(".modal").classList.toggle("wide", wide);
  $("#modal-overlay").classList.remove("hidden");
}
function closeModal() { $("#modal-overlay").classList.add("hidden"); }
$("#modal-close").addEventListener("click", closeModal);
$("#modal-overlay").addEventListener("click", (e) => {
  if (e.target.id === "modal-overlay") closeModal();
});

// ---------- Nhan hien thi ----------
const NHOM_LABEL = { THUC_AN: "Thức ăn", NUOC_UONG: "Nước uống", SPA: "Spa / Massage", KHAC: "Khác" };
const TT_PHONG_LABEL = { TRONG: "Trống", DANG_O: "Đang ở", BAO_TRI: "Bảo trì" };
const TT_DAT_LABEL = { DA_DAT: "Đã đặt", DANG_O: "Đang ở", DA_TRA: "Đã trả", DA_HUY: "Đã hủy" };

// ---------- Ngay ----------
function isoToVN(iso) {
  if (!iso) return "";
  const [y, m, d] = iso.split("-");
  return `${d}/${m}/${y}`;
}
function vnToISO(vn) {
  if (!vn) return "";
  const [d, m, y] = vn.split("/");
  return `${y}-${m.padStart(2, "0")}-${d.padStart(2, "0")}`;
}
function todayISO() { return new Date().toISOString().slice(0, 10); }
function addDaysISO(iso, n) {
  const dt = new Date(iso + "T00:00:00");
  dt.setDate(dt.getDate() + n);
  return dt.toISOString().slice(0, 10);
}

// ---------- Field helper ----------
function field(label, inputNode, full = false) {
  return el("div", { class: "field" + (full ? " full" : "") },
    el("label", {}, label), inputNode);
}
function input(name, opts = {}) {
  return el("input", { name, ...opts });
}
function select(name, options, value) {
  const s = el("select", { name });
  for (const [v, t] of options) {
    const o = el("option", { value: v }, t);
    if (v === value) o.selected = true;
    s.append(o);
  }
  return s;
}
function formData(form) {
  return Object.fromEntries(new FormData(form).entries());
}

// =====================================================
//  VIEW: PHÒNG
// =====================================================
async function renderRooms() {
  const wrap = $("#room-grid");
  wrap.innerHTML = "<div class='empty'>Đang tải…</div>";
  const filter = $("#room-filter").value;
  let rooms;
  try { rooms = await api("/rooms"); }
  catch (e) { wrap.innerHTML = `<div class='empty'>${e.message}</div>`; return; }

  const counts = { TRONG: 0, DANG_O: 0, BAO_TRI: 0 };
  rooms.forEach((r) => counts[r.trangThai]++);
  $("#room-summary").innerHTML = "";
  $("#room-summary").append(
    el("div", { class: "chip" }, el("b", {}, String(rooms.length)), el("span", {}, "Tổng số phòng")),
    el("div", { class: "chip" }, el("b", {}, String(counts.TRONG)), el("span", {}, "Phòng trống")),
    el("div", { class: "chip" }, el("b", {}, String(counts.DANG_O)), el("span", {}, "Đang có khách")),
    el("div", { class: "chip" }, el("b", {}, String(counts.BAO_TRI)), el("span", {}, "Đang bảo trì")),
  );

  const list = rooms.filter((r) => !filter || r.trangThai === filter);
  wrap.innerHTML = "";
  if (!list.length) { wrap.innerHTML = "<div class='empty'>Không có phòng phù hợp.</div>"; return; }

  for (const r of list) {
    const actions = el("div", { class: "rc-actions" });
    if (r.trangThai === "TRONG") {
      actions.append(el("button", {
        class: "btn small primary",
        onclick: () => openBookingForm(r.maPhong),
      }, "Đặt phòng"));
    }
    actions.append(
      el("button", { class: "btn small", onclick: () => openRoomForm(r) }, "Sửa"),
      el("button", { class: "btn small danger", onclick: () => delRoom(r.maPhong) }, "Xóa"),
    );

    wrap.append(el("div", { class: "room-card st-" + r.trangThai },
      el("div", { class: "rc-top" },
        el("span", { class: "rc-code" }, r.maPhong),
        el("span", { class: "badge " + r.trangThai }, TT_PHONG_LABEL[r.trangThai] || r.trangThai),
      ),
      el("div", { class: "rc-type" }, `Loại ${r.loaiPhong} · sức chứa ${r.sucChua} người`),
      el("div", { class: "rc-price" }, money(r.donGia) + " / đêm"),
      r.moTa ? el("div", { class: "rc-meta" }, r.moTa) : null,
      r.luotDat ? el("div", { class: "rc-guest" },
        el("strong", {}, r.luotDat.tenKhach), el("br"),
        `${r.luotDat.ngayNhan} → ${r.luotDat.ngayTra} · ${TT_DAT_LABEL[r.luotDat.trangThai]}`,
      ) : null,
      actions,
    ));
  }
}

function openRoomForm(room = null) {
  const edit = !!room;
  const form = el("form", { class: "form-grid" },
    field("Mã phòng", input("maPhong", { value: room?.maPhong || "", required: true, ...(edit ? { readonly: true } : {}) })),
    field("Loại phòng", input("loaiPhong", { value: room?.loaiPhong || "", placeholder: "A / B / C…" })),
    field("Đơn giá / đêm (₫)", input("donGia", { type: "number", min: "0", value: room?.donGia ?? 0, required: true })),
    field("Sức chứa (người)", input("sucChua", { type: "number", min: "1", value: room?.sucChua ?? 2 })),
    edit ? field("Trạng thái", select("trangThai",
      [["TRONG", "Trống"], ["BAO_TRI", "Bảo trì"]], room.trangThai === "DANG_O" ? "" : room.trangThai)) : null,
    field("Mô tả", input("moTa", { value: room?.moTa || "" }), true),
    el("div", { class: "form-foot full" },
      el("button", { type: "button", class: "btn", onclick: closeModal }, "Hủy"),
      el("button", { type: "submit", class: "btn primary" }, edit ? "Lưu" : "Thêm phòng"),
    ),
  );
  form.addEventListener("submit", async (e) => {
    e.preventDefault();
    const d = formData(form);
    try {
      if (edit) await api(`/rooms/${encodeURIComponent(room.maPhong)}/update`, "POST", d);
      else await api("/rooms", "POST", d);
      toast(edit ? "Đã cập nhật phòng" : "Đã thêm phòng", "ok");
      closeModal(); renderRooms();
    } catch (err) { toast(err.message, "err"); }
  });
  openModal(edit ? "Sửa phòng " + room.maPhong : "Thêm phòng mới", form);
}

async function delRoom(ma) {
  if (!confirm(`Xóa phòng ${ma}?`)) return;
  try { await api(`/rooms/${encodeURIComponent(ma)}/delete`, "POST", {}); toast("Đã xóa phòng", "ok"); renderRooms(); }
  catch (e) { toast(e.message, "err"); }
}

// =====================================================
//  VIEW: ĐẶT PHÒNG
// =====================================================
async function renderBookings() {
  const tb = $("#booking-table tbody");
  tb.innerHTML = "<tr><td colspan='9' class='empty'>Đang tải…</td></tr>";
  const filter = $("#booking-filter").value;
  let list;
  try { list = await api("/bookings" + (filter ? "?trangThai=" + filter : "")); }
  catch (e) { tb.innerHTML = `<tr><td colspan='9' class='empty'>${e.message}</td></tr>`; return; }

  tb.innerHTML = "";
  if (!list.length) { tb.innerHTML = "<tr><td colspan='9' class='empty'>Chưa có lượt đặt nào.</td></tr>"; return; }

  for (const b of list) {
    const act = el("div", { class: "row-actions" });
    if (b.trangThai === "DA_DAT") {
      act.append(
        el("button", { class: "btn small ok", onclick: () => doAction(`/bookings/${b.maDat}/checkin`, "Đã check-in") }, "Check-in"),
        el("button", { class: "btn small", onclick: () => openServiceUse(b) }, "Dịch vụ"),
        el("button", { class: "btn small danger", onclick: () => doAction(`/bookings/${b.maDat}/cancel`, "Đã hủy lượt đặt", true) }, "Hủy"),
      );
    } else if (b.trangThai === "DANG_O") {
      act.append(
        el("button", { class: "btn small", onclick: () => openServiceUse(b) }, "Dịch vụ"),
        el("button", { class: "btn small", onclick: () => showInvoice(b.maDat) }, "Hóa đơn"),
        el("button", { class: "btn small ok", onclick: () => checkout(b.maDat) }, "Check-out"),
      );
    } else {
      act.append(el("button", { class: "btn small", onclick: () => showInvoice(b.maDat) }, "Hóa đơn"));
    }

    tb.append(el("tr", {},
      el("td", {}, b.maDat),
      el("td", {}, el("strong", {}, b.tenKhach), el("br"),
        el("span", { class: "rc-type" }, [b.cccd, b.sdt].filter(Boolean).join(" · "))),
      el("td", {}, `${b.maPhong} (${b.loaiPhong})`),
      el("td", {}, b.ngayNhan),
      el("td", {}, b.ngayTra),
      el("td", { class: "num" }, String(b.soDem)),
      el("td", { class: "num" }, money(b.tienCoc)),
      el("td", {}, el("span", { class: "badge " + b.trangThai }, TT_DAT_LABEL[b.trangThai] || b.trangThai)),
      el("td", {}, act),
    ));
  }
}

async function doAction(path, okMsg, needConfirm = false) {
  if (needConfirm && !confirm("Xác nhận thao tác này?")) return;
  try { await api(path, "POST", {}); toast(okMsg, "ok"); renderBookings(); renderRooms(); }
  catch (e) { toast(e.message, "err"); }
}

async function openBookingForm(maPhongChon = "") {
  let rooms;
  try { rooms = await api("/rooms"); } catch (e) { toast(e.message, "err"); return; }
  const trong = rooms.filter((r) => r.trangThai === "TRONG");
  if (!trong.length) { toast("Không còn phòng trống", "err"); return; }

  const t = todayISO();
  const form = el("form", { class: "form-grid" },
    field("Phòng", select("maPhong", trong.map((r) => [r.maPhong, `${r.maPhong} · Loại ${r.loaiPhong} · ${money(r.donGia)}/đêm`]), maPhongChon), true),
    field("Tên khách hàng", input("tenKhach", { required: true }), true),
    field("CCCD / CMND", input("cccd", {})),
    field("Số điện thoại", input("sdt", {})),
    field("Ngày nhận phòng", input("_nhan", { type: "date", value: t, required: true })),
    field("Ngày trả phòng", input("_tra", { type: "date", value: addDaysISO(t, 1), required: true })),
    field("Tiền cọc (₫)", input("tienCoc", { type: "number", min: "0", value: 0 }), true),
    el("div", { class: "form-foot full" },
      el("button", { type: "button", class: "btn", onclick: closeModal }, "Hủy"),
      el("button", { type: "submit", class: "btn primary" }, "Tạo đặt phòng"),
    ),
  );
  form.addEventListener("submit", async (e) => {
    e.preventDefault();
    const d = formData(form);
    d.ngayNhan = isoToVN(d._nhan); d.ngayTra = isoToVN(d._tra);
    delete d._nhan; delete d._tra;
    try {
      await api("/bookings", "POST", d);
      toast("Đã tạo lượt đặt phòng", "ok");
      closeModal(); renderBookings(); renderRooms();
    } catch (err) { toast(err.message, "err"); }
  });
  openModal("Tạo đặt phòng mới", form);
}
$("#btn-add-booking").addEventListener("click", () => openBookingForm());

async function checkout(maDat) {
  if (!confirm("Xác nhận check-out và lập hóa đơn?")) return;
  try {
    const r = await api(`/bookings/${maDat}/checkout`, "POST", {});
    toast("Đã check-out", "ok");
    renderBookings(); renderRooms();
    renderInvoice(r.data, "Hóa đơn thanh toán - " + maDat);
  } catch (e) { toast(e.message, "err"); }
}

async function showInvoice(maDat) {
  try {
    const hd = await api(`/bookings/${maDat}/bill`);
    if (hd.ok === false) throw new Error(hd.thongBao);
    renderInvoice(hd, "Hóa đơn - " + maDat);
  } catch (e) { toast(e.message, "err"); }
}

function renderInvoice(hd, title) {
  const box = el("div", {});
  box.append(
    el("div", { class: "invoice-line" }, el("span", {}, "Khách hàng"), el("strong", {}, hd.tenKhach)),
    el("div", { class: "invoice-line" }, el("span", {}, "Phòng"), el("span", {}, `${hd.maPhong} (Loại ${hd.loaiPhong})`)),
    el("div", { class: "invoice-line" }, el("span", {}, "Thời gian"), el("span", {}, `${hd.ngayNhan} → ${hd.ngayTra} · ${hd.soDem} đêm`)),
    el("div", { class: "invoice-line" }, el("span", {}, `Tiền phòng (${money(hd.donGiaPhong)} × ${hd.soDem})`), el("span", {}, money(hd.tienPhong))),
  );
  box.append(el("div", { class: "invoice-sub" }, "Dịch vụ đã dùng"));
  if (hd.dichVu && hd.dichVu.length) {
    hd.dichVu.forEach((s) => box.append(
      el("div", { class: "invoice-line" },
        el("span", {}, `${s.tenDV} × ${s.soLuong}`), el("span", {}, money(s.thanhTien))),
    ));
  } else {
    box.append(el("div", { class: "invoice-line" }, el("span", {}, "Không sử dụng dịch vụ"), el("span", {}, money(0))));
  }
  box.append(
    el("div", { class: "invoice-line" }, el("span", {}, "Cộng tiền dịch vụ"), el("span", {}, money(hd.tienDichVu))),
    el("div", { class: "invoice-line" }, el("span", {}, "Tiền cọc đã nhận"), el("span", {}, "− " + money(hd.tienCoc))),
    el("div", { class: "invoice-line total" }, el("span", {}, "TỔNG CỘNG"), el("span", {}, money(hd.tongCong))),
    el("div", { class: "invoice-line due" }, el("span", {}, "CÒN PHẢI THANH TOÁN"), el("span", {}, money(hd.conLai))),
    el("div", { class: "form-foot" }, el("button", { class: "btn primary", onclick: closeModal }, "Đóng")),
  );
  openModal(title, box);
}

// --- Modal them dich vu cho luot dat ---
async function openServiceUse(b) {
  let services, used;
  try {
    [services, used] = await Promise.all([api("/services"), api(`/bookings/${b.maDat}/services`)]);
  } catch (e) { toast(e.message, "err"); return; }

  const usedWrap = el("div", {});
  const drawUsed = (rows) => {
    usedWrap.innerHTML = "";
    if (!rows.length) { usedWrap.append(el("div", { class: "empty" }, "Chưa dùng dịch vụ nào.")); return; }
    let tong = 0;
    rows.forEach((s) => {
      tong += s.thanhTien;
      usedWrap.append(el("div", { class: "invoice-line" },
        el("span", {}, `${s.tenDV} × ${s.soLuong} · ${s.ngay}`),
        el("span", {},
          money(s.thanhTien) + "  ",
          b.trangThai === "DA_TRA" ? null : el("button", {
            class: "btn small danger",
            onclick: async () => {
              try { await api(`/service-orders/${s.maSD}/delete`, "POST", {}); reload(); }
              catch (e) { toast(e.message, "err"); }
            },
          }, "×")),
      ));
    });
    usedWrap.append(el("div", { class: "invoice-line total" }, el("span", {}, "Cộng"), el("span", {}, money(tong))));
  };
  drawUsed(used);

  const form = el("form", { class: "form-grid" },
    field("Dịch vụ", select("maDV", services.map((s) => [s.maDV, `${s.tenDV} · ${money(s.donGia)}/${s.donViTinh}`])), true),
    field("Số lượng", input("soLuong", { type: "number", min: "1", value: 1 })),
    field("Ngày dùng", input("_ngay", { type: "date", value: todayISO() })),
    el("div", { class: "form-foot full" },
      el("button", { type: "submit", class: "btn primary" }, "+ Thêm dịch vụ")),
  );

  async function reload() {
    const rows = await api(`/bookings/${b.maDat}/services`);
    drawUsed(rows);
    renderBookings();
  }
  form.addEventListener("submit", async (e) => {
    e.preventDefault();
    const d = formData(form);
    d.ngay = isoToVN(d._ngay); delete d._ngay;
    try { await api(`/bookings/${b.maDat}/services`, "POST", d); toast("Đã thêm dịch vụ", "ok"); reload(); }
    catch (err) { toast(err.message, "err"); }
  });

  const body = el("div", {},
    el("p", { class: "hint" }, `Lượt đặt ${b.maDat} · ${b.tenKhach} · phòng ${b.maPhong}`),
    b.trangThai === "DA_TRA" ? null : form,
    el("div", { class: "invoice-sub" }, "Đã sử dụng"),
    usedWrap,
    el("div", { class: "form-foot" }, el("button", { class: "btn", onclick: closeModal }, "Đóng")),
  );
  openModal("Dịch vụ bổ sung", body, true);
}

// =====================================================
//  VIEW: DỊCH VỤ
// =====================================================
async function renderServices() {
  const tb = $("#service-table tbody");
  tb.innerHTML = "<tr><td colspan='8' class='empty'>Đang tải…</td></tr>";
  const filter = $("#service-filter").value;
  let list;
  try { list = await api("/services"); }
  catch (e) { tb.innerHTML = `<tr><td colspan='8' class='empty'>${e.message}</td></tr>`; return; }

  const view = list.filter((s) => !filter || s.nhom === filter);
  tb.innerHTML = "";
  if (!view.length) { tb.innerHTML = "<tr><td colspan='8' class='empty'>Không có dịch vụ phù hợp.</td></tr>"; return; }

  for (const s of view) {
    tb.append(el("tr", {},
      el("td", {}, s.maDV),
      el("td", {}, s.tenDV),
      el("td", {}, NHOM_LABEL[s.nhom] || s.nhom),
      el("td", { class: "num" }, money(s.donGia)),
      el("td", { class: "num" }, money(s.giaVon)),
      el("td", { class: "num" }, money(s.donGia - s.giaVon)),
      el("td", {}, s.donViTinh),
      el("td", {}, el("div", { class: "row-actions" },
        el("button", { class: "btn small", onclick: () => openServiceForm(s) }, "Sửa"),
        el("button", { class: "btn small danger", onclick: () => delService(s.maDV) }, "Xóa"),
      )),
    ));
  }
}

function openServiceForm(sv = null) {
  const edit = !!sv;
  const form = el("form", { class: "form-grid" },
    field("Tên dịch vụ", input("tenDV", { value: sv?.tenDV || "", required: true }), true),
    field("Nhóm", select("nhom", Object.entries(NHOM_LABEL), sv?.nhom || "KHAC")),
    field("Đơn vị tính", input("donViTinh", { value: sv?.donViTinh || "lần" })),
    field("Đơn giá bán (₫)", input("donGia", { type: "number", min: "0", value: sv?.donGia ?? 0, required: true })),
    field("Giá vốn (₫)", input("giaVon", { type: "number", min: "0", value: sv?.giaVon ?? 0 })),
    el("div", { class: "form-foot full" },
      el("button", { type: "button", class: "btn", onclick: closeModal }, "Hủy"),
      el("button", { type: "submit", class: "btn primary" }, edit ? "Lưu" : "Thêm"),
    ),
  );
  form.addEventListener("submit", async (e) => {
    e.preventDefault();
    const d = formData(form);
    try {
      if (edit) await api(`/services/${sv.maDV}/update`, "POST", d);
      else await api("/services", "POST", d);
      toast(edit ? "Đã cập nhật dịch vụ" : "Đã thêm dịch vụ", "ok");
      closeModal(); renderServices();
    } catch (err) { toast(err.message, "err"); }
  });
  openModal(edit ? "Sửa dịch vụ " + sv.maDV : "Thêm dịch vụ", form);
}

async function delService(ma) {
  if (!confirm(`Xóa dịch vụ ${ma}?`)) return;
  try { await api(`/services/${ma}/delete`, "POST", {}); toast("Đã xóa dịch vụ", "ok"); renderServices(); }
  catch (e) { toast(e.message, "err"); }
}

// =====================================================
//  VIEW: THỐNG KÊ
// =====================================================
async function renderStats() {
  const tu = $("#stat-from").value ? isoToVN($("#stat-from").value) : "";
  const den = $("#stat-to").value ? isoToVN($("#stat-to").value) : "";
  const qs = [];
  if (tu) qs.push("tu=" + encodeURIComponent(tu));
  if (den) qs.push("den=" + encodeURIComponent(den));

  let s;
  try { s = await api("/stats" + (qs.length ? "?" + qs.join("&") : "")); }
  catch (e) { toast(e.message, "err"); return; }

  const kpi = $("#stat-kpi");
  kpi.innerHTML = "";
  const card = (label, val, hl = false) =>
    el("div", { class: "kpi" + (hl ? " hl" : "") }, el("span", {}, label), el("b", {}, val));
  kpi.append(
    card("Tổng lợi nhuận", money(s.tongLoiNhuan), true),
    card("Tổng doanh thu", money(s.tongDoanhThu)),
    card("Doanh thu phòng", money(s.doanhThuPhong)),
    card("Doanh thu dịch vụ", money(s.doanhThuDichVu)),
    card("Giá vốn dịch vụ", money(s.giaVonDichVu)),
    card("Lợi nhuận dịch vụ", money(s.loiNhuanDichVu)),
    card("Số lượt khách", String(s.soLuotKhach)),
    card("Tổng số đêm bán", String(s.tongDem)),
  );

  const rt = $("#stat-room-table tbody");
  rt.innerHTML = "";
  if (!s.theoLoaiPhong.length) rt.innerHTML = "<tr><td colspan='4' class='empty'>Chưa có dữ liệu</td></tr>";
  s.theoLoaiPhong.forEach((r) => rt.append(el("tr", {},
    el("td", {}, "Loại " + r.loaiPhong),
    el("td", { class: "num" }, money(r.doanhThu)),
    el("td", { class: "num" }, String(r.soLuot)),
    el("td", { class: "num" }, String(r.soDem)),
  )));

  const st = $("#stat-service-table tbody");
  st.innerHTML = "";
  if (!s.theoNhomDichVu.length) st.innerHTML = "<tr><td colspan='5' class='empty'>Chưa có dữ liệu</td></tr>";
  s.theoNhomDichVu.forEach((r) => st.append(el("tr", {},
    el("td", {}, NHOM_LABEL[r.nhom] || r.nhom),
    el("td", { class: "num" }, money(r.doanhThu)),
    el("td", { class: "num" }, money(r.giaVon)),
    el("td", { class: "num" }, money(r.loiNhuan)),
    el("td", { class: "num" }, String(r.soLuong)),
  )));
}

$("#btn-stat-run").addEventListener("click", renderStats);
$("#btn-stat-all").addEventListener("click", () => {
  $("#stat-from").value = ""; $("#stat-to").value = ""; renderStats();
});
$("#btn-stat-month").addEventListener("click", () => {
  const now = new Date();
  const first = new Date(now.getFullYear(), now.getMonth(), 1);
  const last = new Date(now.getFullYear(), now.getMonth() + 1, 0);
  $("#stat-from").value = first.toISOString().slice(0, 10);
  $("#stat-to").value = last.toISOString().slice(0, 10);
  renderStats();
});

// =====================================================
//  Điều hướng tab
// =====================================================
const RENDER = { rooms: renderRooms, bookings: renderBookings, services: renderServices, stats: renderStats };
function goTab(name) {
  if (!RENDER[name]) name = "rooms";
  $$(".tab").forEach((b) => b.classList.toggle("active", b.dataset.view === name));
  $$(".view").forEach((v) => v.classList.remove("active"));
  $("#view-" + name).classList.add("active");
  if (location.hash !== "#" + name) history.replaceState(null, "", "#" + name);
  RENDER[name]();
}
$$(".tab").forEach((btn) => btn.addEventListener("click", () => goTab(btn.dataset.view)));
window.addEventListener("hashchange", () => goTab(location.hash.slice(1)));

$("#btn-add-room").addEventListener("click", () => openRoomForm());
$("#btn-add-service").addEventListener("click", () => openServiceForm());
$("#room-filter").addEventListener("change", renderRooms);
$("#booking-filter").addEventListener("change", renderBookings);
$("#service-filter").addEventListener("change", renderServices);

// Khởi động - mở tab theo hash trên URL (vd .../#stats), mặc định là "rooms"
goTab(location.hash.slice(1) || "rooms");
