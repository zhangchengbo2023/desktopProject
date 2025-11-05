#include <QObject>
#include <QMutex>

class DetectionSystem : public QObject {
    Q_OBJECT

public:
    // 获取单例实例的静态方法
    static DetectionSystem* getInstance() {
        static QMutex mutex;
        QMutexLocker locker(&mutex);  // 保证线程安全
        static DetectionSystem instance;  // 唯一实例
        return &instance;
    }

    // 禁用复制构造函数和赋值操作符
    DetectionSystem(const DetectionSystem&) = delete;
    DetectionSystem& operator=(const DetectionSystem&) = delete;

    enum class State {
        Idle,
        Detecting
    };

    State getCurrentState() const {
        return currentState;
    }

    void startDetection() {
        if (currentState == State::Idle) {
            currentState = State::Detecting;
            emit stateChanged(currentState);
        }
    }

    void stopDetection() {
        if (currentState == State::Detecting) {
            currentState = State::Idle;
            emit stateChanged(currentState);
        }
    }



signals:
    void stateChanged(State newState);

private:
    DetectionSystem(QObject *parent = nullptr) : QObject(parent), currentState(State::Idle) {}

    State currentState;
};
