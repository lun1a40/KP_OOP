// Дашборд: сводные счётчики и разбивка по категориям.

async function loadDashboard() {
    try {
        const [assets, depreciation] = await Promise.all([
            API.get('/api/assets'),
            API.get('/api/reports/depreciation'),   // onDate по умолчанию — сегодня
        ]);

        renderCards(assets, depreciation.totals);
        renderByCategory(assets, depreciation.rows);
    } catch (e) {
        showError(e.message);
    }
}

function renderCards(assets, totals) {
    const byStatus = { InUse: 0, InRepair: 0, WrittenOff: 0 };
    assets.forEach(a => { byStatus[a.status] = (byStatus[a.status] || 0) + 1; });

    const cards = [
        { label: 'Всего фондов', value: assets.length },
        { label: 'В работе',     value: byStatus.InUse },
        { label: 'В ремонте',    value: byStatus.InRepair },
        { label: 'Списано',      value: byStatus.WrittenOff },
        { label: 'Первонач. стоимость', value: formatMoney(totals.initialCost), small: true },
        { label: 'Остаточная стоимость', value: formatMoney(totals.residual), small: true },
    ];

    document.getElementById('cards').innerHTML = cards.map(c => `
        <div class="card">
            <div class="label">${c.label}</div>
            <div class="value ${c.small ? 'small' : ''}">${c.value}</div>
        </div>`).join('');
}

function renderByCategory(assets, rows) {
    // Остаточную стоимость берём из отчёта об амортизации (по assetId).
    const residualById = {};
    rows.forEach(r => { residualById[r.assetId] = r.residual; });

    const agg = {};
    assets.forEach(a => {
        const c = a.category;
        if (!agg[c]) agg[c] = { count: 0, initial: 0, residual: 0 };
        agg[c].count += 1;
        agg[c].initial += a.initialCost;
        agg[c].residual += (residualById[a.id] || 0);
    });

    const tbody = document.getElementById('by-category');
    const keys = Object.keys(agg);
    if (keys.length === 0) {
        tbody.innerHTML = '<tr><td colspan="4" class="empty">Фондов пока нет</td></tr>';
        return;
    }
    tbody.innerHTML = keys.map(c => `
        <tr>
            <td>${categoryLabel(c)}</td>
            <td class="num">${agg[c].count}</td>
            <td class="num">${formatMoney(agg[c].initial)}</td>
            <td class="num">${formatMoney(agg[c].residual)}</td>
        </tr>`).join('');
}

loadDashboard();
