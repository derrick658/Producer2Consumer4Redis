
为了更详细地说明**聚合（Aggregate）**和**仓储（Repository）**的实现，接下来我们会以股票订单系统为例，深入展示如何定义聚合以及如何通过仓储模式与数据库进行交互。

 

### 1. **聚合的详细实现**：

 

**聚合**通常是多个相关对象（实体和值对象）的组合，而**聚合根**是聚合的入口点，外部只能通过聚合根来操作聚合内的其他对象。

 

#### 场景：订单聚合

 

一个股票订单（`Order`）系统中，订单不仅仅是一个简单的实体，它可能包括多个相关的操作和数据，如订单的状态、执行记录等。在这里，`Order`是聚合根，它负责管理整个订单的生命周期和执行记录。

 

#### 订单聚合的代码实现：

 

```go

// Price 是值对象

type Price struct {

    Value float64

}

 

// Quantity 是值对象

type Quantity struct {

    Value int

}

 

// StockSymbol 是值对象

type StockSymbol struct {

    Value string

}

 

// Execution 是订单的执行记录，它是一个独立的实体

type Execution struct {

    ID              string    // 执行的唯一标识

    ExecutionPrice  Price     // 执行价格

    ExecutedAt      time.Time // 执行时间

    ExecutedQuantity Quantity  // 执行数量

}

 

// Order 是聚合根，代表一个股票订单

type Order struct {

    ID         string       // 订单ID，聚合根的唯一标识

    Stock      StockSymbol  // 股票代码

    Price      Price        // 订单价格

    Quantity   Quantity     // 订单数量

    Status     string       // 订单状态（Created, Executed, Cancelled）

    Executions []Execution  // 订单的执行记录

}

 

// 订单聚合根的方法，用于执行订单

func (o *Order) ExecuteOrder(price Price, quantity Quantity) {

    execution := Execution{

        ID:              generateExecutionID(),

        ExecutionPrice:  price,

        ExecutedAt:      time.Now(),

        ExecutedQuantity: quantity,

    }

    o.Executions = append(o.Executions, execution)

    o.Status = "Executed"

}

 

// 生成唯一的执行ID

func generateExecutionID() string {

    return "EXEC-" + strconv.Itoa(rand.Intn(100000))

}

```

 

在上面的代码中：

- `Order`是聚合根，包含了订单的基础信息（如价格、数量）以及执行的记录`Executions`。

- `ExecuteOrder`方法用于执行订单，它创建一个新的执行记录并将其添加到`Executions`中，同时更新订单的状态。

- 聚合根的设计确保了外部只能通过`Order`实体来操作订单，无法直接修改`Executions`，从而保持了数据的一致性。

 

### 2. **仓储模式的详细实现**：

 

**仓储（Repository）**是对持久化的抽象，用来管理聚合的持久化操作。仓储负责将聚合从数据库加载到内存中，或者将聚合的变化保存回数据库。

 

#### 场景：订单仓储

 

我们需要为订单创建一个仓储，它负责以下任务：

- 保存订单（包括订单本身和执行记录）。

- 根据订单ID查询订单。

- 更新订单的状态或执行情况。

 

#### 仓储接口定义：

 

```go

// OrderRepository 定义与订单聚合相关的仓储操作

type OrderRepository interface {

    Save(order *Order) error               // 保存新订单

    FindByID(id string) (*Order, error)     // 根据ID查找订单

    Update(order *Order) error              // 更新订单

}

```

 

#### 仓储的SQL实现：

 

假设我们使用SQL数据库来存储订单和执行记录，可以实现一个`SqlOrderRepository`，它通过SQL语句与数据库交互。

 

```go

type SqlOrderRepository struct {

    db *sql.DB  // 数据库连接

}

 

// Save 实现保存订单及其执行记录

func (r *SqlOrderRepository) Save(order *Order) error {

    // 事务处理确保原子性

    tx, err := r.db.Begin()

    if err != nil {

        return err

    }

 

    // 保存订单

    _, err = tx.Exec("INSERT INTO orders (id, stock_symbol, price, quantity, status) VALUES (?, ?, ?, ?, ?)",

        order.ID, order.Stock.Value, order.Price.Value, order.Quantity.Value, order.Status)

    if err != nil {

        tx.Rollback()

        return err

    }

 

    // 保存执行记录

    for _, exec := range order.Executions {

        _, err = tx.Exec("INSERT INTO executions (id, order_id, execution_price, executed_at, executed_quantity) VALUES (?, ?, ?, ?, ?)",

            exec.ID, order.ID, exec.ExecutionPrice.Value, exec.ExecutedAt, exec.ExecutedQuantity.Value)

        if err != nil {

            tx.Rollback()

            return err

        }

    }

 

    return tx.Commit()

}

 

// FindByID 实现根据ID查找订单

func (r *SqlOrderRepository) FindByID(id string) (*Order, error) {

    var order Order

 

    // 查找订单基本信息

    row := r.db.QueryRow("SELECT id, stock_symbol, price, quantity, status FROM orders WHERE id = ?", id)

    err := row.Scan(&order.ID, &order.Stock.Value, &order.Price.Value, &order.Quantity.Value, &order.Status)

    if err != nil {

        return nil, err

    }

 

    // 查找执行记录

    rows, err := r.db.Query("SELECT id, execution_price, executed_at, executed_quantity FROM executions WHERE order_id = ?", id)

    if err != nil {

        return nil, err

    }

    defer rows.Close()

 

    for rows.Next() {

        var exec Execution

        err = rows.Scan(&exec.ID, &exec.ExecutionPrice.Value, &exec.ExecutedAt, &exec.ExecutedQuantity.Value)

        if err != nil {

            return nil, err

        }

        order.Executions = append(order.Executions, exec)

    }

 

    return &order, nil

}

 

// Update 实现订单的更新操作

func (r *SqlOrderRepository) Update(order *Order) error {

    // 事务处理确保原子性

    tx, err := r.db.Begin()

    if err != nil {

        return err

    }

 

    // 更新订单状态

    _, err = tx.Exec("UPDATE orders SET status = ? WHERE id = ?", order.Status, order.ID)

    if err != nil {

        tx.Rollback()

        return err

    }

 

    // 更新或插入新的执行记录（这里可以假设只有新的执行记录需要添加）

    for _, exec := range order.Executions {

        _, err = tx.Exec("INSERT INTO executions (id, order_id, execution_price, executed_at, executed_quantity) VALUES (?, ?, ?, ?, ?) ON DUPLICATE KEY UPDATE execution_price = VALUES(execution_price), executed_at = VALUES(executed_at), executed_quantity = VALUES(executed_quantity)",

            exec.ID, order.ID, exec.ExecutionPrice.Value, exec.ExecutedAt, exec.ExecutedQuantity.Value)

        if err != nil {

            tx.Rollback()

            return err

        }

    }

 

    return tx.Commit()

}

```

 

### 3. **如何使用聚合和仓储**：

 

当你需要对订单进行操作时，应用程序通过仓储加载聚合，并执行相关的业务逻辑，最后通过仓储保存回数据库。

 

#### 示例：执行订单

 

```go

func executeOrder(repo OrderRepository, orderID string, executionPrice float64, executionQuantity int) error {

    // 从仓储中查找订单

    order, err := repo.FindByID(orderID)

    if err != nil {

        return err

    }

 

    // 执行订单操作

    order.ExecuteOrder(Price{executionPrice}, Quantity{executionQuantity})

 

    // 更新订单

    err = repo.Update(order)

    if err != nil {

        return err

    }

 

    return nil

}

```

 

### 总结：

1. **聚合根**负责管理整个聚合内部的对象，确保一致性和完整性。外部只能通过聚合根访问和操作聚合。

2. **仓储模式**通过提供简化的接口来管理聚合的持久化，使应用层与数据库操作解耦。仓储封装了数据的存取逻辑，并通过事务保证数据的一致性。

3. **数据库交互**在聚合内，通过仓储的实现与数据库进行连接，可以使用SQL等方式操作数据库，保证实体和聚合的数据存储。

 

通过这种模式，领域逻辑和数据持久化得到了清晰的分离，业务逻辑的维护也变得更加容易。

