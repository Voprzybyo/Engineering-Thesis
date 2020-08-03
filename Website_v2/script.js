var scope, speed, increment, flag;

self.onInit = function() {
    scope = self.ctx.$scope;
    speed = 0;
    increment = 0;
}

self.onDataUpdated = function() {
    if(typeof self.ctx.defaultSubscription.data[0].data[0] !== 'undefined') {
        scope.status = self.ctx.defaultSubscription.data[0].data[0][1];
        self.ctx.detectChanges();
        if(scope.status == "ON") {
            window.globalFlag = true;
            flag = false;
            setTimeout(function() {
                window.requestAnimationFrame(actionStatus);
            });
        }
    }
}

function actionStatus() {
    speed += increment;
    $('.svg_1').css('transform', 'rotate(' + speed + 'deg)');
    if(scope.status == 'ON') {
        if(increment < 10) {
            increment += 0.025;
        }
    } else if(scope.status == "OFF") {
        if (!flag) {
            flag = true;
        }
        if (increment > 0) {
            increment -= 0.025;
        } else {
            return;
        }
    }
    self.ctx.detectChanges();
    if (window.globalFlag) {
        window.requestAnimationFrame(actionStatus);
    } else {
        return;
    }
}

self.typeParameters = function() {
    return {
        maxDatasources: 1,
        maxDataKeys: 1
    };
}

self.onDestroy = function() {
    window.globalFlag = false;
}